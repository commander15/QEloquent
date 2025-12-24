#ifndef QELOQUENT_CONNECTION_H
#define QELOQUENT_CONNECTION_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

class QSqlDatabase;
class QSqlQuery;
class QSqlError;
class QUrl;

namespace QEloquent {

class Driver;

class ConnectionData;
class QELOQUENT_EXPORT Connection
{
public:
    Connection();
    Connection(const Connection &);
    Connection(Connection &&);
    Connection &operator=(const Connection &);
    Connection &operator=(Connection &&);
    ~Connection();

    QString name() const;

    bool isOpen() const;
    bool open();
    bool open(const QString &user, const QString &password);
    void close();

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    QSqlQuery exec(const QString &query, bool cache = false);
    QSqlError lastError() const;

    Driver *driver() const;

    const QSqlDatabase database() const;
    QSqlDatabase database();

    bool isValid() const;

    operator const QSqlDatabase() const;
    operator QSqlDatabase();

    static Connection connection(const QString &name);
    static Connection addConnection(const QString &name, const QUrl &url);
    static Connection addConnection(const QString &name, const QString &type, const QString &dbName, int port = -1);
    static Connection addConnection(const QString &name, const QString &type, const QString &dbName, int port, const QString &userName, const QString &password);
    static Connection addConnection(const QString &name, const QSqlDatabase &db, bool ownDb = false);
    static void removeConnection(const QString &name);

    static QString defaultConnectionName();
    static Connection defaultConnection();
    static void setDefault(const Connection &connection);

private:
    Connection(ConnectionData *data);

    QExplicitlySharedDataPointer<ConnectionData> data;

    static QString s_defaultConnection;
    static QMap<QString, Connection> s_connections;
};

} // namespace QEloquent

#endif // QELOQUENT_CONNECTION_H
