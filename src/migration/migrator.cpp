#include "migrator.h"
#include "migrator_p.h"

#include <QEloquent/migration.h>
#include <QEloquent/error.h>
#include <QEloquent/query.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>

#include <QDebug>
#include <QVariantMap>
#include <QDateTime>
#include <QSqlQuery>

namespace QEloquent {

Result<bool, Error> Migrator::init(const QString &connectionName)
{
    const QString tableName = QStringLiteral("migrations");
    try {
        Schema::create("migrations")
    }
}

Result<int, Error> Migrator::migrate()
{
    int executed = 0;

    for (int i(0); i < storage->migrations.size(); ++i) {
        Migration *migration = storage->migrations.at(i);
        Schema::setConnection(migration->connectionName());

        // We skip if migration already run
        if (migration->isExecuted()) continue;

        try {
            migration->up();
            updateMigration(migration, Executed);
            ++executed;
        } catch (const SchemaException &e) {
#ifdef QT_DEBUG
            logError("Failled to run migration '" + migration->name() + '\'', e);
#endif
            return unexpected(Error(Error::DatabaseError, e.what()));
        }
    }

    return executed;
}

Result<int, Error> Migrator::rollback(int steps)
{
    int executed = 0;

    for (int i(storage->migrations.size() - 1); steps > 0 && i > 0; --i) {
        Migration *migration = storage->migrations.at(i);
        Schema::setConnection(migration->connectionName());
        updateMigration(migration, Unknown);

        // We stop if migration not executed
        if (!migration->isExecuted()) return 0;

        try {
            migration->down();
            updateMigration(migration, Unexecuted);
            ++executed;
            --steps;
        } catch (const SchemaException &e) {
#ifdef QT_DEBUG
            logError("Failled to rollback migration '" + migration->name() + '\'', e);
#endif
            return unexpected(Error(Error::DatabaseError, e.what()));
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

bool Migrator::updateMigration(Migration *migration, Event event)
{
    Query query;
    query.table("migrations").connection(migration->connectionName());

    Result<QSqlQuery, QSqlError> result;

    if (event == Unknown) {
        auto result = QueryRunner::select("id", query.where("name", migration->name()));
        if (result && result->next())
            migration->m_id = result->value(0).toInt();
    } else if (event == Executed) {
        const QVariantMap values = {
            { "name", migration->name() },
            { "timestamp", QDateTime::currentDateTime() }
        };

        result = QueryRunner::insert(values, query);
        if (result)
            migration->m_id = result->lastInsertId().toInt();
    } else if (event == Unexecuted) {
        result = QueryRunner::deleteData(query.where("id", migration->id()));
        if (result)
            migration->m_id = 0;
    }

    return result.has_value();
}

void Migrator::logError(const QString &text, const SchemaException &e)
{
    qDebug()
            .noquote()
            .nospace()
        << text
        << ", executed statement:\n" << e.statement << '\n'
        << ", error: " << e.error;
}

QScopedPointer<MigratorStorage> Migrator::storage(new MigratorStorage());

} // namespace QEloquent
