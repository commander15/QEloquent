#include "connection.h"

#include <QMap>
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
};

Connection::Connection()
    : data{new ConnectionData()}
{}

Connection::Connection(const Connection &rhs)
    : data{rhs.data}
{}

Connection::Connection(Connection &&rhs)
    : data{std::move(rhs.data)}
{}

Connection::Connection(ConnectionData *data)
    : data(data)
{}

Connection &Connection::operator=(const Connection &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

Connection &Connection::operator=(Connection &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

Connection::~Connection()
{}

QString Connection::name() const
{
    return data->connectionName;
}

bool Connection::isOpen() const
{
    return database().isOpen();
}

bool Connection::open()
{
    return database().open();
}

bool Connection::open(const QString &user, const QString &password)
{
    return database().open(user, password);
}

void Connection::close()
{
    database().close();
}

bool Connection::beginTransaction()
{
    return database().transaction();
}

bool Connection::commitTransaction()
{
    return database().commit();
}

bool Connection::rollbackTransaction()
{
    return database().rollback();
}

QSqlQuery Connection::exec(const QString &query, bool cache)
{
    QSqlQuery q(database());
    q.setForwardOnly(!cache);
    q.exec(query);
    return q;
}

QSqlError Connection::lastError() const
{
    return database().lastError();
}

const QSqlDatabase Connection::database() const
{
    return QSqlDatabase::database(data->databaseConnectionName, false);
}

QSqlDatabase Connection::database()
{
    return QSqlDatabase::database(data->databaseConnectionName, false);
}

bool Connection::isValid() const
{
    return database().isValid();
}

Connection::operator const QSqlDatabase() const
{
    return database();
}

Connection::operator QSqlDatabase()
{
    return database();
}

Connection Connection::connection(const QString &name)
{
    if (s_connections.contains(name))
        return s_connections.value(name);
    else
        return Connection(new ConnectionData());
}

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

Connection Connection::addConnection(const QString &name, const QString &type, const QString &dbName, int port)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(type, name);
    db.setDatabaseName(dbName);
    if (port > 0)
        db.setPort(port);

    return addConnection(name, db, true);
}

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

Connection Connection::addConnection(const QString &name, const QSqlDatabase &db, bool ownDb)
{
    ConnectionData *data = new ConnectionData();
    data->connectionName = name;
    data->databaseConnectionName = db.connectionName();
    data->databaseConnectionOwned = ownDb;

    Connection con(data);
    s_connections.insert(name, con);

    if (s_defaultConnection.isEmpty())
        s_defaultConnection = name;

    return con;
}

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

Connection Connection::defaultConnection()
{
    if (s_defaultConnection.isEmpty())
        return Connection();
    else
        return s_connections.value(s_defaultConnection);
}

void Connection::setDefault(const Connection &connection)
{
    s_defaultConnection = connection.name();
}

QString Connection::s_defaultConnection;
QMap<QString, Connection> Connection::s_connections;

} // namespace QEloquent
