#include "connection.h"

#include <QEloquent/error.h>
#include <QEloquent/driver.h>

#include <QDateTime>
#include <QTimeZone>
#include <QElapsedTimer>
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

    QDateTime lastNow;
    QElapsedTimer nowTimer;

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

Result<QDateTime, Error> Connection::now(TimePrecision precision) const
{
    // Determine refresh interval
    qint64 intervalMs;
    switch (precision) {
    case TimePrecision::DefaultPrecision:    intervalMs = 3600000LL;  break;  // 1 hour
    case TimePrecision::HighPrecision:       intervalMs = 10000LL;    break;  // 10 seconds
    case TimePrecision::RealTimePrecision:   intervalMs = 60000LL;    break;  // 1 minute
    case TimePrecision::LowPrecision:        intervalMs = 21600000LL; break;  // 6 hours
    default:                                 intervalMs = 3600000LL;  break;  // fallback
    }

    // Cache hit: return adjusted cached time
    if (data->lastNow.isValid() &&
        data->nowTimer.isValid() &&
        data->nowTimer.elapsed() < intervalMs)
    {
        return data->lastNow.addMSecs(data->nowTimer.elapsed());
    }

    // Cache miss or stale: refresh from DB
    const QString stmt = "SELECT " + data->driver->currentTimestamp();
    auto result = exec(stmt, false);

    if (!result || !result->next()) {
        return failWith(Error::fromSqlError(result.error()));
    }

    QDateTime dbNow = result->value(0).toDateTime();
    if (!dbNow.isValid()) {
        return failWith(Error(Error::DatabaseError, "Invalid timestamp from DB"));
    }
    dbNow.setTimeZone(QTimeZone::utc());

    // Update cache
    data->lastNow = dbNow;
    if (data->nowTimer.isValid())
        data->nowTimer.restart();
    else
        data->nowTimer.start();

    return dbNow;
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
        return failWith(q.lastError());
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
    const QString driver = url.scheme().toLower();

    // Database name / file path from url path
    QString path = url.path();

    // When using SQLite in memory db, the '/' on path is not needed (Linux only)
    // Also, for the others, it's even unwanted (since not file path based)
    if ((driver == "sqlite" && path.startsWith("/:")) || driver != "sqlite")
        path.remove(0, 1);

    // Creating connection instance
    Connection con = addConnection(name, driver, path, url.userName(), url.password(), url.host(), url.port());

    if (url.hasQuery()) {
        const QUrlQuery query(url.query());
        QSqlDatabase db = con.database();

        // Add connection options (if any)
        if (query.hasQueryItem("options"))
            db.setConnectOptions(query.queryItemValue("options"));

        // Add numerical preceision (if defined)
        if (query.hasQueryItem("numerical_precision")) {
            const QString precesion = query.queryItemValue("numerical_precision");
            if (precesion == "high")
                db.setNumericalPrecisionPolicy(QSql::HighPrecision);
            else if (precesion == "low")
                db.setNumericalPrecisionPolicy(QSql::LowPrecisionDouble);
        }
    }

    // Returning the connection
    return con;
}

/*!
 * @brief Adds a new connection with basic parameters.
 */
Connection Connection::addConnection(const QString &name, const QString &type, const QString &dbName)
{
    return addConnection(name, type, dbName, QString(), QString(), QString(), -1);
}

Connection Connection::addConnection(const QString &name, const QString &type, const QString &dbName, const QString &hostName, int port)
{
    return addConnection(name, type, dbName, QString(), QString(), hostName, port);
}

/*!
 * @brief Adds a new connection with full credentials.
 */
Connection Connection::addConnection(const QString &name, const QString &type, const QString &dbName, const QString &userName, const QString &password, const QString &hostName, int port)
{
    const QString driver = (type.startsWith("Q") ? type : 'Q' + type).toUpper();
    QSqlDatabase db;
    if (name.isEmpty())
        db = QSqlDatabase::addDatabase(driver);
    else
        db = QSqlDatabase::addDatabase(driver, name);

    db.setDatabaseName(dbName);

    if (!userName.isEmpty())
        db.setUserName(userName);

    if (!password.isEmpty())
        db.setPassword(password);

    if (!hostName.isEmpty())
        db.setHostName(hostName);

    if (port > 0)
        db.setPort(port);

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

    if (s_defaultConnection.isNull())
        s_defaultConnection = name;

    return con;
}

/*!
 * @brief Removes a connection by name.
 */
void Connection::removeConnection(const QString &name)
{
    if (!s_connections.contains(name)) {
        return;
    }

    const Connection con = s_connections.take(name);

    // Warn if another instance exists
    if (con.data->ref > 1) {
        qWarning().noquote() << "Removing connection" << name
                             << "which seems to be used somewhere";
    }

    if (s_defaultConnection == name)
        s_defaultConnection = QString();

    if (con.data->databaseConnectionOwned)
        QSqlDatabase::removeDatabase(con.data->databaseConnectionName);
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
    if (s_defaultConnection.isNull())
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
