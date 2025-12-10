#include "queryrunner.h"

#include <QEloquent/connection.h>

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

Result<QSqlQuery, QSqlError> QueryRunner::exec(const QString &statement)
{
    return exec(statement, Connection::defaultConnection());
}

Result<QSqlQuery, QSqlError> QueryRunner::exec(const QString &statement, const QString &connectionName)
{
    return exec(statement, Connection::connection(connectionName));
}

Result<QSqlQuery, QSqlError> QueryRunner::exec(const QString &statement, const Connection &connection)
{
    QSqlQuery query(connection.database());
    query.setForwardOnly(true);
    if (query.exec(statement))
        return query;
    else
        return unexpected(query.lastError());
}

} // namespace QEloquent
