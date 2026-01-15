#include "migrator.h"
#include "migrator_p.h"

#include <QEloquent/migration.h>
#include <QEloquent/error.h>
#include <QEloquent/query.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>
#include <QEloquent/datamap.h>

#include <QDebug>
#include <QVariant>
#include <QDateTime>
#include <QSqlQuery>

#define MIGRATIONS_TABLE "migrations"

namespace QEloquent {

Result<int, Error> Migrator::init(const QString &connectionName)
{
    return prepareMigrations();
}

Result<int, Error> Migrator::migrate(const MigrationMonitor &monitor)
{
    auto preparation = prepareMigrations();
    if (!preparation)
        return failWith(preparation.error());

    int executed = 0;

    for (int i(0); i < storage->migrations.size(); ++i) {
        Migration *migration = storage->migrations.at(i);
        Schema::setConnection(migration->connectionName());

        // We skip if migration already run
        if (migration->isExecuted()) continue;

        try {
            // Monitor before run
            if (monitor) monitor(migration, true);

            // Run
            migration->up();

            // Save run
            auto result = saveMigrationRecord(migration, true);

            // If run saving failed, we rollback the migration and fail with an error
            if (!result) {
                migration->down();
                return failWith(result.error());
            }

            ++executed;
        } catch (const SchemaException &e) {
#ifdef QT_DEBUG
            logError("Failled to run migration '" + migration->name() + '\'', e);
#endif
            return failWith(Error(Error::DatabaseError, e.what(), e.error));
        }
    }

    return executed;
}

Result<int, Error> Migrator::rollback(int steps, const MigrationMonitor &monitor)
{
    auto preparation = prepareMigrations();
    if (!preparation)
        return failWith(preparation.error());

    int executed = 0;

    for (int i(storage->migrations.size() - 1); i >= 0 && steps > 0; --i) {
        Migration *migration = storage->migrations.at(i);
        Schema::setConnection(migration->connectionName());

        // We stop if migration not executed
        if (!migration->isExecuted()) return 0;

        try {
            // Monitor before run
            if (monitor) monitor(migration, false);

            // Run
            migration->down();

            // Save run
            auto result = saveMigrationRecord(migration, false);

            // If run saving failed, we revert (up) the migration and fail with an error
            if (!result) {
                migration->up();
                return failWith(result.error());
            }

            ++executed;
            --steps;
        } catch (const SchemaException &e) {
#ifdef QT_DEBUG
            logError("Failled to rollback migration '" + migration->name() + '\'', e);
#endif
            return failWith(Error(Error::DatabaseError, e.what()));
        }
    }

    return executed;
}

Result<int, Error> Migrator::refresh(const MigrationMonitor &monitor)
{
    int executed = 0;

    // Revert all migrations
    auto rollbackResult = rollback(storage->migrations.count(), monitor);
    if (rollbackResult) executed += rollbackResult.value();
    else return rollbackResult;

    // Run all migrations
    auto migrateResult = migrate(monitor);
    if (migrateResult) executed += migrateResult.value();
    else return migrateResult;

    return executed;
}

void Migrator::registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down, const QDateTime &creationTime)
{
    Migration *migration = Migration::create(name, up, down, creationTime, Connection::defaultConnectionName());
    registerMigration(migration);
}

void Migrator::registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down, const QDateTime &creationTime, const QString &connectionName)
{
    Migration *migration = Migration::create(name, up, down, creationTime, connectionName);
    registerMigration(migration);
}

void Migrator::registerMigration(Migration *migration)
{
    storage->add(migration);
}

void Migrator::clear()
{
    storage->clearMigrations();
}

Result<int, Error> Migrator::prepareMigrations()
{
    // We create the migrations table if it doesn't exists already
    if (!Schema::hasTable(MIGRATIONS_TABLE)) {
        auto result = createMigrationsTable();
        if (!result) return result;
    }

    // We get migrations data
    return retrieveMigrationsData();
}

Result<bool, Error> Migrator::createMigrationsTable()
{
    try {
        Schema::create(MIGRATIONS_TABLE, [](TableBlueprint &table) {
            table.id();
            table.string("name", 30).unique();
            table.timestamp("created_at");
            table.timestamp("executed_at");
        });

        return true;
    } catch (const SchemaException &e) {
        return failWith(Error(Error::DatabaseError, e.statement + "\n" + e.error.text(), e.error));
    }
}

Result<int, Error> Migrator::retrieveMigrationsData()
{
    Query query;
    query.table(MIGRATIONS_TABLE);
    auto result = QueryRunner::select(QStringList() << "id" << "name" << "executed_at", query);
    if (!result) return failWith(Error(Error::DatabaseError, QString(), result.error()));

    int count = 0;

    while (result->next()) {
        const QString name = result->value(1).toString();

        auto it = std::find_if(
            storage->migrations.begin(), storage->migrations.end(),
            [&name](Migration *migration) {
                return migration->name() == name;
        });

        if (it == storage->migrations.end())
            continue;

        const int id = result->value(0).toInt();
        const QDateTime execTime = result->value(2).toDateTime();
        (*it)->updateData(id, execTime);

        ++count;
    }

    return count;
}

Result<int, Error> Migrator::saveMigrationRecord(Migration *migration, bool executed)
{
    Query query = Query().table(MIGRATIONS_TABLE);

    // We delete after de-exec
    if (!executed) {
        auto result = QueryRunner::deleteData(query.where("id", migration->id()));
        if (result)
            return -1;
        else
            return failWith(Error(Error::DatabaseError, QString(), result.error()));
    }

    // We insert after exec

    auto nowResult = Connection::defaultConnection().now();
    if (!nowResult) return failWith(nowResult.error());

    DataMap data = {
        { "name", migration->name() },
        { "executed_at", nowResult.value() },
        { "created_at", migration->createdAt() },
    };

    auto result = QueryRunner::insert(data, query);
    if (!result)
        return failWith(Error(Error::DatabaseError, QString(), result.error()));

    migration->m_id = result->lastInsertId().toInt();
    return migration->id();
}

void Migrator::logError(const QString &text, const SchemaException &e)
{
    qDebug()
            .noquote()
            .nospace()
        << text
        << ", executed statement:\n" << e.statement << '\n'
        << ", error: " << e.error.text();
}

QScopedPointer<MigratorStorage> Migrator::storage(new MigratorStorage());

} // namespace QEloquent
