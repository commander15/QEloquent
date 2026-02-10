#include "migration.h"
#include "migration_p.h"

#include <QEloquent/connection.h>
#include <QEloquent/migrator.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/query.h>
#include <QEloquent/error.h>
#include <QEloquent/datamap.h>
#include <QEloquent/private/migrator_p.h>

#include <QFile>
#include <QSqlQuery>

#include <QList>

namespace QEloquent {

/*!
 * \class Migration
 * \brief Base class for database migrations in QEloquent.
 *
 * A migration defines a reversible change to the database schema or data.
 * Each migration has an \c up() method to apply the change and a \c down()
 * method to revert it.
 *
 * Migrations are registered with \c Migrator and executed in order.
 * The framework tracks which migrations have been applied using a dedicated
 * tracking table.
 *
 * \sa Migrator for managing and executing migrations.
 */

/*!
 * \brief Destroys the migration object.
 */
Migration::~Migration() = default;

/*!
 * \brief Returns the unique identifier of the migration.
 *
 * This ID is assigned when the migration is saved to the tracking table.
 *
 * \return The migration's numeric ID, or 0 if not yet executed.
 */
int Migration::id() const
{
    return m_id;
}

/*!
 * \fn QString Migration::name
 * \brief Returns the migration name.
 */

/*!
 * \brief Returns the timestamp when the migration was executed.
 *
 * \return The execution time (UTC), or an invalid QDateTime if not yet executed.
 */
QDateTime Migration::executedAt() const
{
    return m_executedAt;
}

/*!
 * \brief Checks whether the migration has been applied to the database.
 *
 * \return \c true if the migration has an ID and a valid execution timestamp,
 *         \c false otherwise.
 */
bool Migration::isExecuted() const
{
    return m_id > 0 && m_executedAt.isValid();
}

/*!
 * \fn void Migration::up()
 * \brief Applies the migration (the "up" direction).
 *
 * This pure virtual method must be implemented by concrete migration classes.
 * It contains the logic to modify the database schema or data in the forward direction.
 *
 * Typical operations include:
 * - Creating tables or columns
 * - Adding indexes, constraints, or foreign keys
 * - Inserting or updating seed data
 * - Running arbitrary SQL statements
 *
 * The method is called by \c Migrator when applying pending migrations.
 *
 * \note This method should be idempotent where possible (safe to run multiple times without side effects).
 *       Use conditional checks (e.g. \c Schema::hasTable(), \c Schema::hasColumn()) if needed.
 *
 * \sa down() for the reverse operation.
 */

/*!
 * \fn void Migration::down()
 * \brief Reverts the migration (the "down" direction).
 *
 * This pure virtual method must be implemented by concrete migration classes.
 * It contains the logic to undo the changes made by \c up().
 *
 * Typical operations include:
 * - Dropping tables or columns
 * - Removing indexes, constraints, or foreign keys
 * - Deleting or reverting seed data
 * - Running arbitrary rollback SQL statements
 *
 * The method is called by \c Migrator when rolling back applied migrations.
 *
 * \note
 * - The down operation should be the exact inverse of up() whenever possible.
 * - Use \c Schema::dropIfExists() and similar safe methods to avoid errors.
 * - Data loss may occur during rollback if not handled carefully (e.g., dropping columns).
 *
 * \sa up() for the forward operation.
 */

/*!
 * \brief Returns the name of the connection this migration should use.
 *
 * Defaults to the default connection.
 *
 * Subclasses may override to use a specific connection.
 *
 * \return The connection name.
 */
QString Migration::connectionName() const
{
    return Connection::defaultConnectionName();
}

/*!
 * \brief Returns the active connection object for this migration.
 *
 * Uses \c connectionName() to retrieve the correct connection.
 *
 * \return The Connection object associated with this migration.
 */
Connection Migration::connection() const
{
    return Connection::connection(connectionName());
}

/*!
 * \brief Factory method to create a new migration.
 *
 * Creates a generic migration with the given name and callbacks.
 * If auto-registration is enabled, the migration is automatically
 * registered with the \c Migrator.
 *
 * \param name Unique name of the migration (used for tracking).
 * \param up Callback to apply the migration (the "up" direction).
 * \param down Callback to revert the migration (the "down" direction).
 * \return A pointer to the newly created migration.
 */
Migration *Migration::create(const QString &name, const Callback &up, const Callback &down)
{
    Migration *m = new GenericMigration(name, up, down, Connection::defaultConnectionName());
    if (s_autoRegistrationOn) Migrator::registerMigration(m);
    return m;
}

/*!
 * \brief Factory method to create a new migration with explicit connection.
 *
 * Same as the overload without \c connectionName, but allows specifying
 * which database connection to use.
 *
 * \param name Unique name of the migration.
 * \param up Callback to apply the migration.
 * \param down Callback to revert the migration.
 * \param connectionName Name of the connection to use.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::create(const QString &name, const Callback &up, const Callback &down, const QString &connectionName)
{
    Migration *m = new GenericMigration(name, up, down, connectionName);
    if (s_autoRegistrationOn) Migrator::registerMigration(m);
    return m;
}

/*!
 * \brief Convenience factory to create a migration that creates a table.
 *
 * Generates a migration with \c up() calling \c Schema::create() and
 * \c down() calling \c Schema::dropIfExists().
 *
 * Uses the default connection.
 *
 * \param tableName Name of the table to create.
 * \param callback Blueprint callback to define the table structure.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::createTable(const QString &tableName, const Schema::BlueprintCallback &callback)
{
    return createTable(tableName, callback, Connection::defaultConnectionName());
}

/*!
 * \brief Convenience factory to create a migration that creates a table on a specific connection.
 *
 * Same as the overload without \c connectionName, but allows specifying
 * the target database connection.
 *
 * \param tableName Name of the table to create.
 * \param callback Blueprint callback to define the table structure.
 * \param connectionName Name of the connection to use.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::createTable(const QString &tableName, const Schema::BlueprintCallback &callback, const QString &connectionName)
{
    auto up = [tableName, callback] {
        Schema::create(tableName, callback);
    };
    auto down = [tableName] {
        Schema::dropIfExists(tableName);
    };
    return create("create_" + tableName + "_table", up, down, connectionName);
}

/*!
 * \brief Factory to create a migration from script files using a filename pattern.
 *
 * Expects two files: \a filePrefix_up.sql and \a filePrefix_down.sql.
 * Uses the default connection.
 *
 * \param name Unique name of the migration.
 * \param filePrefix Common prefix of the up/down script files.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::fromScriptFilePattern(const QString &name, const QString &filePrefix)
{
    return fromScriptFiles(name, filePrefix + "_up.sql", filePrefix + "_down.sql", Connection::defaultConnectionName());
}

/*!
 * \brief Factory to create a migration from script files using a filename pattern on a specific connection.
 *
 * \param name Unique name of the migration.
 * \param filePrefix Common prefix of the up/down script files.
 * \param connectionName Name of the connection to use.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::fromScriptFilePattern(const QString &name, const QString &filePrefix, const QString &connectionName)
{
    return fromScriptFiles(name, filePrefix + "_up.sql", filePrefix + "_down.sql", connectionName);
}

/*!
 * \brief Factory to create a migration from script files.
 *
 * Reads the up and down SQL scripts and converts them to callbacks.
 * Uses the default connection.
 *
 * \param name Unique name of the migration.
 * \param upFileName Path to the "up" SQL script file.
 * \param downFileName Path to the "down" SQL script file.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName)
{
    return fromScriptFiles(name, upFileName, downFileName, Connection::defaultConnectionName());
}

/*!
 * \brief Factory to create a migration from script files on a specific connection.
 *
 * \param name Unique name of the migration.
 * \param upFileName Path to the "up" SQL script file.
 * \param downFileName Path to the "down" SQL script file.
 * \param connectionName Name of the connection to use.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::fromScriptFiles(const QString &name, const QString &upFileName, const QString &downFileName, const QString &connectionName)
{
    QByteArray up;
    QByteArray down;
    QFile file;
    file.setFileName(upFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        up = file.readAll();
        file.close();
    }
    file.setFileName(downFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        down = file.readAll();
        file.close();
    }
    return fromScriptContents(name, up, down, connectionName);
}

/*!
 * \brief Factory to create a migration from raw SQL script contents.
 *
 * Converts the script contents into executable callbacks.
 * Uses the default connection.
 *
 * \param name Unique name of the migration.
 * \param upScript Raw SQL content for the "up" direction.
 * \param downScript Raw SQL content for the "down" direction.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript)
{
    return fromScriptContents(name, upScript, downScript, Connection::defaultConnectionName());
}

/*!
 * \brief Factory to create a migration from raw SQL script contents on a specific connection.
 *
 * Splits the scripts into statements and wraps them in callbacks.
 *
 * \param name Unique name of the migration.
 * \param upScript Raw SQL content for the "up" direction.
 * \param downScript Raw SQL content for the "down" direction.
 * \param connectionName Name of the connection to use.
 * \return A pointer to the newly created migration.
 */
Migration *Migration::fromScriptContents(const QString &name, const QByteArray &upScript, const QByteArray &downScript, const QString &connectionName)
{
    const Callback up = [upScript] {
        QStringList statements = QueryBuilder::statementsFromScriptContent(upScript);
        Schema::exec(statements);
    };
    const Callback down = [downScript] {
        QStringList statements = QueryBuilder::statementsFromScriptContent(downScript);
        Schema::exec(statements);
    };
    return create(name, up, down, connectionName);
}

/*!
 * \brief Enables automatic registration of migrations created via factory methods.
 *
 * When enabled, any migration created with \c create(), \c createTable(),
 * or \c fromScript...() methods is automatically registered with \c Migrator.
 */
void Migration::enableAutoRegistration()
{
    s_autoRegistrationOn = true;
}

/*!
 * \brief Disables automatic registration of migrations.
 *
 * Migrations created via factory methods will not be registered automatically.
 * The caller must manually call \c Migrator::registerMigration().
 */
void Migration::disableAutoRegistration()
{
    s_autoRegistrationOn = false;
}

/*!
 * \brief Updates the internal tracking data after execution.
 *
 * Called by \c Migrator after successful execution to set the ID and timestamp.
 *
 * \param id The ID assigned in the tracking table.
 * \param execTime The UTC timestamp when the migration was executed.
 */
void Migration::updateData(int id, const QDateTime &execTime)
{
    m_id = id;
    m_executedAt = execTime;
}

/*!
 * \brief Marks this migration as executed in the tracking table.
 *
 * Saves or updates the migration record to reflect successful execution.
 */
void Migration::markAsExecuted()
{
    auto result = Migrator::saveMigrationRecord(this, true);
    Q_UNUSED(result);
}

/*!
 * \brief Marks this migration as unexecuted in the tracking table.
 *
 * Updates the migration record to reflect that it has been rolled back.
 */
void Migration::markAsUnexecuted()
{
    auto result = Migrator::saveMigrationRecord(this, false);
    Q_UNUSED(result);
}

/*!
 * \brief Internal helper to register a migration.
 *
 * Forwards the registration to \c Migrator.
 *
 * \param migration The migration object to register.
 */
void _registerMigration(Migration *migration)
{
    Migrator::registerMigration(migration);
}

/*!
 * \brief Static flag controlling auto-registration behavior.
 *
 * \c true when factory methods automatically register migrations.
 */
bool Migration::s_autoRegistrationOn = false;

}
