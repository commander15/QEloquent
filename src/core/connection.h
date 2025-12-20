#ifndef QELOQUENT_CONNECTION_H
#define QELOQUENT_CONNECTION_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

class QSqlDatabase;
class QSqlQuery;
class QSqlError;
class QUrl;

namespace QEloquent {

class ConnectionData;
/**
 * @brief Manages database connections and transactions.
 * 
 * Connection is a thin wrapper around QSqlDatabase that provides easy management
 * of multiple named connections and support for default connections.
 */
class QELOQUENT_EXPORT Connection
{
public:
    /** @brief Constructs a null connection */
    Connection();
    /** @brief Copy constructor */
    Connection(const Connection &);
    /** @brief Move constructor */
    Connection(Connection &&);
    /** @brief Copy assignment operator */
    Connection &operator=(const Connection &);
    /** @brief Move assignment operator */
    Connection &operator=(Connection &&);
    /** @brief Destructor */
    ~Connection();

    /** @brief Returns the name of the connection */
    QString name() const;

    /** @brief Returns true if the connection is currently open */
    bool isOpen() const;
    /** @brief Opens the connection using pre-configured parameters */
    bool open();
    /** @brief Opens the connection with specific credentials */
    bool open(const QString &user, const QString &password);
    /** @brief Closes the connection */
    void close();

    /** @brief Begins a SQL transaction */
    bool beginTransaction();
    /** @brief Commits the current SQL transaction */
    bool commitTransaction();
    /** @brief Rolls back the current SQL transaction */
    bool rollbackTransaction();

    /** @brief Executes a raw SQL query on this connection */
    QSqlQuery exec(const QString &query, bool cache = false);
    /** @brief Returns the last error encountered by this connection */
    QSqlError lastError() const;

    /** @brief Returns the underlaying QSqlDatabase (const) */
    const QSqlDatabase database() const;
    /** @brief Returns the underlaying QSqlDatabase */
    QSqlDatabase database();

    /** @brief Returns true if this is a valid, initialized connection */
    bool isValid() const;

    /** @brief Implicit conversion to QSqlDatabase */
    operator const QSqlDatabase() const;
    /** @brief Implicit conversion to QSqlDatabase */
    operator QSqlDatabase();

    /** @brief Retrieves an existing connection by name */
    static Connection connection(const QString &name);
    /** @brief Adds a new connection using a URL */
    static Connection addConnection(const QString &name, const QUrl &url);
    /** @brief Adds a new connection with basic parameters */
    static Connection addConnection(const QString &name, const QString &type, const QString &dbName, int port = -1);
    /** @brief Adds a new connection with full credentials */
    static Connection addConnection(const QString &name, const QString &type, const QString &dbName, int port, const QString &userName, const QString &password);
    /** @brief Adds a connection from an existing QSqlDatabase instance */
    static Connection addConnection(const QString &name, const QSqlDatabase &db, bool ownDb = false);
    /** @brief Removes a connection by name */
    static void removeConnection(const QString &name);

    /** @brief Returns the default connection instance */
    static Connection defaultConnection();
    /** @brief Sets the default connection */
    static void setDefault(const Connection &connection);

private:
    Connection(ConnectionData *data);

    QExplicitlySharedDataPointer<ConnectionData> data;

    static QString s_defaultConnection;
    static QMap<QString, Connection> s_connections;
};

} // namespace QEloquent

#endif // QELOQUENT_CONNECTION_H
