#include "connection.h"

#include <QEloquent/driver.h>

#include <QDateTime>
#include <QTimeZone>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QUrlQuery>

namespace QEloquent {

class ConnectionData : public QSharedData
{
public:
    QString connectionName;
    QString databaseConnectionName;
    bool databaseConnectionOwned = false;

    Driver *driver = nullptr;
};

/*!
 * @class QEloquent::Connection
 * @brief Manages database connections and transactions.
 *
 * Connection is a thin wrapper around QSqlDatabase that provides easy management
 * of multiple named connections and support for default connections.
 */

/*!
 * @brief Constructs a null connection.
 */
Connection::Connection()
    : data{new ConnectionData()}
{}

/*!
 * @brief Copy constructor.
 */
Connection::Connection(const Connection &rhs)
    : data{rhs.data}
{}

/*!
 * @brief Move constructor.
 */
Connection::Connection(Connection &&rhs)
    : data{std::move(rhs.data)}
{}

Connection::Connection(ConnectionData *data)
    : data(data)
{}

/*!
 * @brief Copy assignment operator.
 */
Connection &Connection::operator=(const Connection &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

/*!
 * @brief Move assignment operator.
 */
Connection &Connection::operator=(Connection &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

/*!
 * @brief Destructor.
 */
Connection::~Connection()
{}

/*!
 * @brief Returns the name of the connection.
 */
QString Connection::name() const
{
    return data->connectionName;
}

/*!
 * @brief Returns true if the connection is currently open.
 */
bool Connection::isOpen() const
{
    return database().isOpen();
}

/*!
 * @brief Opens the connection using pre-configured parameters.
 */
bool Connection::open()
{
    return database().open();
}

/*!
 * @brief Opens the connection with specific credentials.
 */
bool Connection::open(const QString &user, const QString &password)
{
    return database().open(user, password);
}

/*!
 * @brief Closes the connection.
 */
void Connection::close()
{
    database().close();
}

/*!
 * @brief Begins a SQL transaction.
 */
bool Connection::beginTransaction()
{
    return database().transaction();
}

/*!
 * @brief Commits the current SQL transaction.
 */
bool Connection::commitTransaction()
{
    return database().commit();
}

/*!
 * @brief Rolls back the current SQL transaction.
 */
bool Connection::rollbackTransaction()
{
    return database().rollback();
}

QDateTime Connection::now() const
{
    const QString statement = "SELECT " + data->driver->timestampDefault();
    auto result = exec(statement, false);
    if (result && result->next()) {
        QDateTime now = result->value(0).toDateTime();
        now.setTimeZone(QTimeZone::UTC);
        return now;
    }

    return QDateTime::currentDateTime();
}

/*!
 * @brief Executes a raw SQL query on this connection.
 */
Result<QSqlQuery, QSqlError> Connection::exec(const QString &query, bool cache) const
{
    QSqlQuery q(database());
    q.setForwardOnly(!cache);
    if (q.exec(query))
        return q;
    else
        return unexpected(q.lastError());
}

/*!
 * @brief Returns the last error encountered by this connection.
 */
QSqlError Connection::lastError() const
{
    return database().lastError();
}

Driver *Connection::driver() const
{
    return data->driver;
}

/*!
 * @brief Returns the underlaying QSqlDatabase (const).
 */
const QSqlDatabase Connection::database() const
{
    return QSqlDatabase::database(data->databaseConnectionName, false);
}

/*!
 * @brief Returns the underlaying QSqlDatabase.
 */
QSqlDatabase Connection::database()
{
    return QSqlDatabase::database(data->databaseConnectionName, false);
}

/*!
 * @brief Returns true if this is a valid, initialized connection.
 */
bool Connection::isValid() const
{
    return database().isValid();
}

/*!
 * @brief Implicit conversion to QSqlDatabase.
 */
Connection::operator const QSqlDatabase() const
{
    return database();
}

/*!
 * @brief Implicit conversion to QSqlDatabase.
 */
Connection::operator QSqlDatabase()
{
    return database();
}

/*!
 * @brief Retrieves an existing connection by name.
 */
Connection Connection::connection(const QString &name)
{
    if (s_connections.contains(name))
        return s_connections.value(name);
    else
        return Connection(new ConnectionData());
}

/*!
 * @brief Adds a new connection using a URL.
 */
Connection Connection::addConnection(const QString &name, const QUrl &url)
{
    // Creating db instance
    const QString driver = 'Q' + url.scheme().toUpper();
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, name);

    // Database name / file path from url path
    QString path = url.path();

    // When using SQLite in memory db, the '/' on path is not needed (Linux only)
    // Also, for the others, it's even unwanted (since not file path based)
    if ((driver == "QSQLITE" && path.startsWith("/:")) || driver != "QSQLITE")
        path.removeFirst();
    db.setDatabaseName(path);

    // Host name from url host
    db.setHostName(url.host());

    // Port from url port
    const int port = url.port();
    if (port > 0)
        db.setPort(port);

    // User credentials from url user infos
    db.setUserName(url.userName());
    db.setPassword(url.password());

    if (url.hasQuery()) {
        const QUrlQuery query(url.query());

        // Add connection options (if any)
        if (query.hasQueryItem("options"))
            db.setConnectOptions(query.queryItemValue("options"));

        // Add numerical preceision (if defined)
        if (query.hasQueryItem("numerical_precision")) {
            const QString precesion = query.queryItemValue("numerical_precision");
            if (precesion == "high")
                db.setNumericalPrecisionPolicy(QSql::HighPrecision);
            else
                db.setNumericalPrecisionPolicy(QSql::LowPrecisionDouble);
        }
    }

    // Registering and returning the connection
    return addConnection(name, db, true);
}

/*!
 * @brief Adds a new connection with basic parameters.
 */
Connection Connection::addConnection(const QString &name, const QString &type, const QString &dbName, int port)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(type, name);
    db.setDatabaseName(dbName);
    if (port > 0)
        db.setPort(port);

    return addConnection(name, db, true);
}

/*!
 * @brief Adds a new connection with full credentials.
 */
Connection Connection::addConnection(const QString &name, const QString &type, const QString &dbName, int port, const QString &userName, const QString &password)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(type, name);
    db.setDatabaseName(dbName);
    if (port > 0)
        db.setPort(port);
    db.setUserName(userName);
    db.setPassword(password);

    return addConnection(name, db, true);
}

/*!
 * @brief Adds a connection from an existing QSqlDatabase instance.
 */
Connection Connection::addConnection(const QString &name, const QSqlDatabase &db, bool ownDb)
{
    ConnectionData *data = new ConnectionData();
    data->connectionName = name;
    data->databaseConnectionName = db.connectionName();
    data->databaseConnectionOwned = ownDb;
    data->driver = Driver::create(db.driverName(), db.driver());

    Connection con(data);
    s_connections.insert(name, con);

    if (s_defaultConnection.isEmpty())
        s_defaultConnection = name;

    return con;
}

/*!
 * @brief Removes a connection by name.
 */
void Connection::removeConnection(const QString &name)
{
    if (s_connections.contains(name)) {
        const Connection con = s_connections.take(name);

        // Warn if another instance exists
        if (con.data->ref > 1) {
            qWarning().noquote() << "Removing connection" << name
                                 << "which seems to be used somewhere";
        }

        if (s_defaultConnection == name)
            s_defaultConnection.clear();

        if (con.data->databaseConnectionOwned)
            QSqlDatabase::removeDatabase(name);
    }
}

QString Connection::defaultConnectionName()
{
    return s_defaultConnection;
}

/*!
 * @brief Returns the default connection instance.
 */
Connection Connection::defaultConnection()
{
    if (s_defaultConnection.isEmpty())
        return Connection();
    else
        return s_connections.value(s_defaultConnection);
}

/*!
 * @brief Sets the default connection.
 */
void Connection::setDefault(const Connection &connection)
{
    s_defaultConnection = connection.name();
}

QString Connection::s_defaultConnection;
QMap<QString, Connection> Connection::s_connections;

} // namespace QEloquent
