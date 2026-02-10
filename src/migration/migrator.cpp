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

/*!
 * \class Migrator
 * \brief Central manager for database migrations in QEloquent.
 *
 * The Migrator class is responsible for:
 * - Registering migrations (either manually or automatically)
 * - Preparing and executing migrations in the correct order
 * - Tracking applied migrations in a dedicated database table
 * - Rolling back applied migrations (reverting changes)
 * - Refreshing the database (full rollback + re-migration)
 *
 * Migrations are executed in the order they are registered.
 * The system ensures idempotency by skipping already-applied migrations.
 *
 * \note All operations are performed on the connection specified by each migration
 *       or the default connection if none is set.
 *
 * \sa Migration for defining individual migrations.
 * \sa Schema for low-level schema manipulation.
 */

/*!
 * \brief Initializes the migrator for the specified connection.
 *
 * Prepares the migration tracking table if it does not exist.
 * This method must be called before any migrations can be executed.
 *
 * \param connectionName The name of the database connection to use.
 *                       If empty, the default connection is used.
 * \return Result containing the number of migrations prepared (usually 0),
 *         or an error if preparation failed.
 */
Result<int, Error> Migrator::init(const QString &connectionName)
{
    return prepareMigrations();
}

/*!
 * \brief Applies all pending migrations.
 *
 * Executes each unregistered (pending) migration in registration order.
 * Skips migrations that have already been applied.
 * Saves execution records after each successful migration.
 * Rolls back the current migration on failure and returns an error.
 *
 * \param monitor Optional callback invoked before each migration runs.
 *                Receives the migration pointer and a boolean indicating direction
 *                (\c true for forward/up, \c false for rollback/down).
 * \return Result containing the number of migrations actually executed,
 *         or an error if any migration failed.
 */
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
            logError("Failed to run migration '" + migration->name() + '\'', e);
#endif
            return failWith(Error(Error::DatabaseError, e.what(), e.error));
        }
    }
    return executed;
}

/*!
 * \brief Reverts the specified number of migration batches.
 *
 * Rolls back migrations in reverse order of application.
 * Stops if a migration has not been executed or if \a steps is reached.
 * Saves rollback records after each successful reversion.
 * Re-applies the migration on failure and returns an error.
 *
 * \param steps Maximum number of batches to roll back (-1 or large number for all).
 * \param monitor Optional callback invoked before each rollback.
 *                Receives the migration pointer and \c false (rollback direction).
 * \return Result containing the number of migrations actually rolled back,
 *         or an error if any rollback failed.
 */
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
        if (!migration->isExecuted()) return executed;
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
            logError("Failed to rollback migration '" + migration->name() + '\'', e);
#endif
            return failWith(Error(Error::DatabaseError, e.what()));
        }
    }
    return executed;
}

/*!
 * \brief Refreshes the database by rolling back and re-applying all migrations.
 *
 * Performs a full rollback followed by a full migration.
 * Useful for testing, development resets, or schema validation.
 *
 * \param monitor Optional callback invoked before each migration/rollback.
 * \return Result containing the total number of operations executed
 *         (rollback + migrate count), or an error if any step failed.
 */
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

/*!
 * \brief Registers a new migration using callbacks.
 *
 * Convenience overload that creates a \c GenericMigration and registers it.
 * Uses the default connection.
 *
 * \param name Unique name of the migration.
 * \param up Callback to apply the migration.
 * \param down Callback to revert the migration.
 */
void Migrator::registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down)
{
    Migration *migration = Migration::create(name, up, down, Connection::defaultConnectionName());
    registerMigration(migration);
}

/*!
 * \brief Registers a new migration using callbacks on a specific connection.
 *
 * \param name Unique name of the migration.
 * \param up Callback to apply the migration.
 * \param down Callback to revert the migration.
 * \param connectionName Name of the connection to use.
 */
void Migrator::registerMigration(const QString &name, const MigrationCallback &up, const MigrationCallback &down, const QString &connectionName)
{
    Migration *migration = Migration::create(name, up, down, connectionName);
    registerMigration(migration);
}

/*!
 * \brief Registers an existing Migration object.
 *
 * Adds the migration to the internal list for future execution.
 * Ownership is transferred to the Migrator.
 *
 * \param migration Pointer to the migration object to register.
 */
void Migrator::registerMigration(Migration *migration)
{
    storage->add(migration);
}

/*!
 * \fn template<typename MigrationClass> static void Migrator::registerMigration()
 * \brief Registers a migration class instance using automatic instantiation.
 *
 * This template convenience method creates a new instance of the specified
 * migration class \c MigrationClass and registers it with the migrator.
 * It is intended for use with concrete migration classes that derive from
 * \c Migration.
 *
 * The migration is instantiated with its default constructor.
 * Ownership is transferred to the migrator.
 *
 * Example usage:
 * \code
 * Migrator::registerMigration<MyCustomMigration>();
 * \endcode
 *
 * \tparam MigrationClass The migration class type (must derive from Migration).
 *
 * \note This method is equivalent to manually creating the instance and calling
 *       the non-template overload:
 *       \code
 *       registerMigration(new MigrationClass());
 *       \endcode
 *
 * \sa registerMigration(Migration*) for the non-template version.
 */

/*!
 * \brief Clears all registered migrations.
 *
 * Removes all migration objects from the internal list.
 * Does not affect the database tracking table.
 */
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
