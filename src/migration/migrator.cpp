#include "migrator.h"
#include "migrator_p.h"

#include <QEloquent/migration.h>
#include <QEloquent/error.h>
#include <QEloquent/query.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>
#include <QEloquent/datamap.h>

#include <QDebug>
#include <QVariant>
#include <QDateTime>
#include <QSqlQuery>

#define MIGRATIONS_TABLE "migrations"

namespace QEloquent {

Result<bool, Error> Migrator::init(const QString &connectionName)
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
        auto result = migration->getStatus(true);
        Schema::setConnection(migration->connectionName());

        // We skip if migration already run
        if (migration->isExecuted()) continue;

        try {
            if (monitor) monitor(migration);
            migration->up();
            auto result = migration->markAsExecuted();
            if (!result) throw SchemaException(QString(), result.error().sqlError());
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

Result<int, Error> Migrator::rollback(int steps)
{
    return rollback(nullptr, steps);
}

Result<int, Error> Migrator::rollback(const MigrationMonitor &monitor, int steps)
{
    auto preparation = prepareMigrations();
    if (!preparation)
        return failWith(preparation.error());

    int executed = 0;

    for (int i(storage->migrations.size() - 1); i >= 0 && steps > 0; --i) {
        Migration *migration = storage->migrations.at(i);
        auto result = migration->getStatus(false);
        if (!result) throw SchemaException(QString(), result.error().sqlError());
        Schema::setConnection(migration->connectionName());

        // We stop if migration not executed
        if (!migration->isExecuted()) return 0;

        try {
            if (monitor) monitor(migration);
            migration->down();
            auto result = migration->markAsUnexecuted();
            if (!result) throw SchemaException(QString(), result.error().sqlError());
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
    auto rollbackResult = rollback(monitor, storage->migrations.count());
    if (rollbackResult) executed += rollbackResult.value();
    else return rollbackResult;

    auto migrateResult = migrate(monitor);
    if (migrateResult) executed += migrateResult.value();
    else return migrateResult;

    return executed;
}

void Migrator::registerMigration(Migration *migration)
{
    storage->add(migration);
}

void Migrator::registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down)
{
    Migration *migration = Migration::create(name, up, down, Connection::defaultConnectionName());
    registerMigration(migration);
}

void Migrator::registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down, const QString &connectionName)
{
    Migration *migration = Migration::create(name, up, down, connectionName);
    registerMigration(migration);
}

void Migrator::clear()
{
    storage->clearMigrations();
}

Result<bool, Error> Migrator::prepareMigrations()
{
    QSqlDatabase db = Connection::defaultConnection().database();

    // We create the migrations table if it doesn't exists already
    if (!Schema::hasTable(MIGRATIONS_TABLE)) {
        try {
            Schema::create(MIGRATIONS_TABLE, [](TableBlueprint &table) {
                table.id();
                table.string("name", 30).unique();
                table.timestamp("executed_at");
            });
        } catch (const SchemaException &e) {
            return failWith(Error(Error::DatabaseError, QString(), e.error));
        }
    }

    // Fetching migrations data
    Query query;
    query.table(MIGRATIONS_TABLE);
    auto result = QueryRunner::select(QStringList() << "id" << "name" << "executed_at", query);
    if (!result) return failWith(Error(Error::DatabaseError, QString(), result.error()));

    while (result->next()) {
        const QString name = result->value(1).toString();
        for (Migration *migration : std::as_const(storage->migrations))
            if (migration->name() == name)
                migration->updateData(result->value(0).toInt(), result->value(2).toDateTime());
    }

    return true;
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
