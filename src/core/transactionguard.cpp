#include "transactionguard.h"

#include <QEloquent/connection.h>

namespace QEloquent {

/**
 * @brief RAII-based guard for managing database transactions safely.
 *
 * @class QEloquent::TransactionGuard
 *
 * The TransactionGuard class acts like a lock guard (`QMutexLocker`) for database
 * transactions. It automatically begins a transaction on construction and ensures an
 * automatic rollback upon destruction if `commit()` has not been explicitly called.
 * This prevents data corruption from early returns, exceptions, or unhandled failures.
 *
 * @code
 * bool processOrder() {
 *     TransactionGuard guard;
 *
 *     if (!guard.transactionStarted()) {
 *         qDebug() << "Failed to start transaction:" << guard.errorString();
 *         return false;
 *     }
 *
 *     bool success = executeOperations();
 *     if (!success) {
 *         // Scope ends -> destructor triggers automatic rollback
 *         return false;
 *     }
 *
 *     if (guard.commit()) {
 *         return true;
 *     } else {
 *         qDebug() << "Commit failed:" << guard.errorString();
 *         return false;
 *     }
 * }
 * @endcode
 */

/**
 * @brief Constructs a transaction guard using the default database connection.
 * Automatically attempts to begin a database transaction.
 */
TransactionGuard::TransactionGuard()
    : TransactionGuard(Connection::defaultConnectionName())
{}

/**
 * @brief Constructs a transaction guard using a specified database connection name.
 * Automatically attempts to begin a database transaction on the given connection.
 *
 * @param connection The name of the database connection (e.g., "sqlite_conn").
 */
TransactionGuard::TransactionGuard(const QString &connection)
    : m_connection(connection)
    , m_commit(false)
{
    Connection con = Connection::connection(connection);
    if (con.beginTransaction()) {
        m_started = true;
    } else {
        m_started = false;
        m_errorCode = con.lastError().nativeErrorCode();
        m_errorString = con.lastError().text();
    }
}

/**
 * @brief Destructor.
 *
 * If the transaction was successfully started and has not been explicitly
 * committed, it automatically rolls back the transaction.
 */
TransactionGuard::~TransactionGuard()
{
    if (m_started && !m_commit) {
        Connection con = Connection::connection(m_connection);
        con.rollbackTransaction();
    }
}

/**
 * @brief Checks whether the database transaction was successfully started.
 *
 * @return true if `beginTransaction()` succeeded, false otherwise.
 */
bool TransactionGuard::transactionStarted() const
{
    return m_started;
}

/**
 * @brief Retrieves the database driver or engine error code associated with a transaction failure.
 *
 * @return QString The error code from the underlying database driver if starting
 *         or committing failed, or an empty string if no error occurred.
 */
QString TransactionGuard::errorCode() const
{
    return m_errorCode;
}

/**
 * @brief Retrieves the last error message associated with transaction failure.
 *
 * @return QString A description of the database error if starting or committing failed.
 */
QString TransactionGuard::errorString() const
{
    return m_errorString;
}

/**
 * @brief Explicitly commits the active database transaction.
 *
 * Marks the transaction for commit and executes `commitTransaction()` on the connection.
 * Once committed, the destructor will not perform a rollback.
 *
 * @return true if the commit succeeded, false if it failed or if the transaction never started.
 */
bool TransactionGuard::commit()
{
    if (!m_started)
        return false;

    Connection con = Connection::connection(m_connection);    
    m_commit = true;

    if (con.commitTransaction()) {
        return true;
    } else {
        m_errorString = con.lastError().text();
        return false;
    }
}

} // namespace QEloquent
