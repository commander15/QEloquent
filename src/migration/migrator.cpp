#include "migrator.h"
#include "migrator_p.h"

#include <QEloquent/migration.h>
#include <QEloquent/error.h>
#include <QEloquent/query.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>
#include <QEloquent/datamap.h>

#include <QDebug>
#include <QVariantMap>
#include <QDateTime>
#include <QSqlQuery>

#define MIGRATIONS_TABLE "migrations"

namespace QEloquent {

Result<bool, Error> Migrator::init(const QString &connectionName)
{
    return prepareMigrations();
}

Result<int, Error> Migrator::migrate()
{
    auto preparation = prepareMigrations();
    if (!preparation)
        return failWith(preparation.error());

    int executed = 0;

    for (int i(0); i < storage->migrations.size(); ++i) {
        Migration *migration = storage->migrations.at(i);
        auto result = migration->getStatus(false);
        Schema::setConnection(migration->connectionName());

        // We skip if migration already run
        if (migration->isExecuted()) continue;

        try {
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
    int executed = 0;

    for (int i(storage->migrations.size() - 1); steps > 0 && i > 0; --i) {
        Migration *migration = storage->migrations.at(i);
        auto result = migration->getStatus(true);
        if (!result) throw SchemaException(QString(), result.error().sqlError());
        Schema::setConnection(migration->connectionName());

        // We stop if migration not executed
        if (!migration->isExecuted()) return 0;

        try {
            migration->down();
            auto result = migration->markAsExecuted();
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

Result<int, Error> Migrator::refresh()
{
    int executed = 0;

    // Revert all migrations
    auto rollbackResult = rollback(storage->migrations.count());
    if (rollbackResult) executed += rollbackResult.value();
    else return rollbackResult;

    auto migrateResult = migrate();
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

    // If the migrations table already exists, we are done
    const QStringList tables = db.tables(QSql::Tables);
    if (tables.contains(MIGRATIONS_TABLE)) return true;

    // We create the migrations table
    try {
        Schema::create(MIGRATIONS_TABLE, [](TableBlueprint &table) {
            table.id();
            table.string("name", 30).unique();
            table.timestamp("executed_at");
        });
    } catch (const SchemaException &e) {
        return failWith(Error(Error::DatabaseError, QString(), e.error));
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
