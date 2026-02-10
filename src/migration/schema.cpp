#include "schema.h"

#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>
#include <QEloquent/driver.h>
#include <QEloquent/schemagrammar.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

namespace QEloquent {

/*!
 * \class Schema
 * \brief High-level facade for schema manipulation in QEloquent.
 *
 * The Schema class provides a simple, static interface for common database schema operations:
 * creating/altering/dropping tables, checking existence, and executing raw statements.
 *
 * All methods use the current connection (or default) and delegate SQL generation to
 * the active SchemaGrammar (driver-specific).
 *
 * \note Schema is stateless and thread-safe for most operations. Connection switching
 *       via \c setConnection() is global and affects all subsequent calls until reset.
 *
 * \note All methods that alter the database structure can throw a SchemaException to report
 *       errors.
 *
 * \sa TableBlueprint for fluent table/column definition.
 * \sa SchemaGrammar for driver-specific SQL generation.
 * \sa Connection for managing database connections.
 */

/*!
 * \brief Checks whether a table exists in the database.
 *
 * Uses the current connection to query the database metadata.
 *
 * \param table Name of the table to check.
 * \return \c true if the table exists, \c false otherwise.
 */
bool Schema::hasTable(const QString &table)
{
    const Connection conn = connection();
    return conn.database().tables().contains(table);
}

/*!
 * \brief Creates a new table using a fluent blueprint callback.
 *
 * \param table Name of the table to create.
 * \param callback Lambda or function that configures the table structure via TableBlueprint.
 */
void Schema::create(const QString &table, const BlueprintCallback &callback)
{
    TableBlueprint blueprint = TableBlueprint::create(table, true, connectionName());
    callback(blueprint);
    const QStringList statements = QueryBuilder::createTableStatements(blueprint, connection());
    exec(statements);
}

/*!
 * \brief Modifies an existing table using a fluent blueprint callback.
 *
 * \param table Name of the table to alter.
 * \param callback Lambda or function that configures alterations via TableBlueprint.
 */
void Schema::table(const QString &table, const BlueprintCallback &callback)
{
    TableBlueprint blueprint = TableBlueprint::create(table, false, connectionName());
    callback(blueprint);
    const QStringList statements = QueryBuilder::alterTableStatements(blueprint, connection());
    exec(statements);
}

/*!
 * \brief Renames an existing table.
 *
 * \param from Current name of the table.
 * \param to New name for the table.
 */
void Schema::rename(const QString &from, const QString &to)
{
    const QStringList statements = grammar()->renameTableStatements(from, to);
    exec(statements);
}

/*!
 * \brief Drops a table from the database.
 *
 * \param table Name of the table to drop.
 */
void Schema::drop(const QString &table)
{
    const QStringList statements = grammar()->dropTableStatements(table, false);
    exec(statements);
}

/*!
 * \brief Drops a table if it exists.
 *
 * Safe alternative to \c drop() — no error if the table is missing.
 *
 * \param table Name of the table to drop.
 */
void Schema::dropIfExists(const QString &table)
{
    const QStringList statements = grammar()->dropTableStatements(table, true);
    exec(statements);
}

/*!
 * \brief Checks whether a column exists in a table.
 *
 * \param column Name of the column to check.
 * \param table Name of the table.
 * \return \c true if the column exists, \c false otherwise.
 */
bool Schema::hasColumn(const QString &column, const QString &table)
{
    return hasColumns(QStringList() << column, table);
}

/*!
 * \brief Checks whether all specified columns exist in a table.
 *
 * \param columns List of column names to check.
 * \param table Name of the table.
 * \return \c true only if all columns exist, \c false otherwise.
 */
bool Schema::hasColumns(const QStringList &columns, const QString &table)
{
    const QSqlRecord record = connection().database().record(table);
    for (const QString &column : columns)
        if (!record.contains(column))
            return false;
    return true;
}

/*!
 * \brief Executes a callback if a column exists in a table.
 *
 * \param column Name of the column to check.
 * \param table Name of the table.
 * \param callback Function to call if the column exists.
 */
void Schema::whenHasColumn(const QString &column, const QString &table, const DefaultCallback &callback)
{
    if (hasColumn(column, table))
        callback();
}

/*!
 * \brief Executes a callback if a column does not exist in a table.
 *
 * \param column Name of the column to check.
 * \param table Name of the table.
 * \param callback Function to call if the column is missing.
 */
void Schema::whenDoesntHaveColumn(const QString &column, const QString &table, const DefaultCallback &callback)
{
    if (!hasColumn(column, table))
        callback();
}

/*!
 * \brief Drops a single column from a table.
 *
 * \param column Name of the column to drop.
 * \param table Name of the table.
 */
void Schema::dropColumn(const QString &column, const QString &table)
{
    dropColumns(QStringList() << column, table);
}

/*!
 * \brief Drops multiple columns from a table.
 *
 * \param columns List of column names to drop.
 * \param table Name of the table.
 */
void Schema::dropColumns(const QStringList &columns, const QString &table)
{
    const SchemaGrammar *grammar = Schema::grammar();
    QStringList statements;
    for (const QString &column : columns)
        statements.append(grammar->dropColumnStatements(column, table));
    exec(statements);
}

/*!
 * \brief Executes a single SQL statement on the current connection.
 *
 * \param statement SQL statement to execute.
 * \return The resulting QSqlQuery object.
 * \throw SchemaException on execution failure.
 */
QSqlQuery Schema::exec(const QString &statement)
{
    auto result = QueryRunner::exec(statement, connection());
    if (result) return std::move(result.value());
    else throw SchemaException(statement, result.error());
}

/*!
 * \brief Executes multiple SQL statements sequentially.
 *
 * \param statements List of SQL statements to execute.
 * \return List of QSqlQuery objects (one per successful statement).
 * \throw SchemaException on the first execution failure.
 */
QList<QSqlQuery> Schema::exec(const QStringList &statements)
{
    const Connection conn = connection();
    QList<QSqlQuery> queries;
    for (const QString &statement : statements) {
        auto result = QueryRunner::exec(statement, conn);
        if (result) queries.append(std::move(result.value()));
        else throw SchemaException(statement, result.error());
    }
    return queries;
}

/*!
 * \brief Returns the name of the current connection used by Schema.
 *
 * If no explicit connection was set, returns the default connection name.
 *
 * \return Current connection name.
 */
QString Schema::connectionName()
{
    if (s_connectionName.isEmpty())
        return Connection::defaultConnectionName();
    else
        return s_connectionName;
}

/*!
 * \brief Returns the current active connection object.
 *
 * Uses \c connectionName() to retrieve the correct connection.
 *
 * \return The active Connection object.
 */
Connection Schema::connection()
{
    if (s_connectionName.isEmpty())
        return Connection::defaultConnection();
    else
        return Connection::connection(s_connectionName);
}

/*!
 * \brief Sets the connection name for subsequent Schema operations.
 *
 * All future Schema calls will use this connection until changed again.
 *
 * \param name Name of the connection to use.
 */
void Schema::setConnection(const QString &name)
{
    s_connectionName = name;
}

SchemaGrammar *Schema::grammar()
{
    const Driver *driver = connection().driver();
    return driver->schemaGrammar();
}

QString Schema::s_connectionName;

/*!
 * \class SchemaException
 * \brief Exception thrown when a schema operation fails.
 *
 * SchemaException is thrown by \c Schema::exec() and related methods when
 * a SQL statement fails to execute. It carries both the failing statement
 * and the underlying \c QSqlError for detailed diagnostics.
 *
 * This exception supports Qt's exception cloning and raising mechanism
 * (via \c raise() and \c clone()), making it compatible with Qt's
 * exception handling in signals/slots and multi-threaded contexts.
 *
 * Typical usage (thrown internally by Schema):
 * \code
 * try {
 *     Schema::create("users", [](TableBlueprint &t) { ... });
 * } catch (const SchemaException &e) {
 *     qCritical() << "Schema error:" << e.what();
 *     qCritical() << "Statement:" << e.statement();
 *     qCritical() << "SQL error:" << e.sqlError().text();
 * }
 * \endcode
 *
 * \sa Schema for schema operations that may throw this exception.
 * \sa QSqlError for the embedded database error details.
 */

/*!
 * \brief Constructs a SchemaException with the failing statement and error.
 *
 * \param statement The SQL statement that failed to execute.
 * \param error The QSqlError object containing database-specific details.
 */
SchemaException::SchemaException(const QString &statement, const QSqlError &error)
    : statement(statement)
    , error(error)
    , m_what(statement.toUtf8() + '\n' + error.text().toUtf8())
{}

/*!
 * \brief Returns a C-string describing the exception.
 *
 * Combines the failing SQL statement and the database error message.
 *
 * \return Null-terminated string suitable for logging or display.
 * \note The returned pointer remains valid as long as the exception object lives.
 */
const char *SchemaException::what() const noexcept
{
    return m_what.data();
}

/*!
 * \brief Re-throws this exception.
 *
 * Required for Qt's exception handling mechanism in signals/slots.
 * Calls \c throw *this.
 */
void SchemaException::raise() const
{
    throw *this;
}

/*!
 * \brief Creates a deep copy of this exception.
 *
 * Required for Qt's exception handling mechanism.
 * Returns a new heap-allocated copy of this exception.
 *
 * \return A pointer to a newly allocated SchemaException copy.
 *         Caller is responsible for deletion.
 */
SchemaException *SchemaException::clone() const
{
    return new SchemaException(*this);
}

} // namespace QEloquent
