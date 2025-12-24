#include "queryrunner.h"

#include <QEloquent/query.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/connection.h>

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

Result<QSqlQuery, QSqlError> QueryRunner::select(const Query &query)
{
    const QString statement = QueryBuilder::selectStatement(query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::select(const QList<QPair<QString, QString> > &fields, const Query &query)
{
    const QString statement = QueryBuilder::selectStatement(fields, query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::select(const QStringList fields, const Query &query)
{
    const QString statement = QueryBuilder::selectStatement(fields, query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::select(const QString &fields, const Query &query)
{
    const QString statement = QueryBuilder::selectStatement(fields, query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::count(const Query &query)
{
    const QString statement = QueryBuilder::selectStatement("COUNT(1)", query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::insert(const QVariantMap &data, const Query &query)
{
    const QString statement = QueryBuilder::insertStatement(data, query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::update(const QVariantMap &data, const Query &query)
{
    const QString statement = QueryBuilder::updateStatement(data, query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::deleteData(const Query &query)
{
    const QString statement = QueryBuilder::deleteStatement(query);
    return exec(statement, query.connection());
}

Result<QSqlQuery, QSqlError> QueryRunner::createTable(const QString &tableName, const TableBlueprint &blueprint)
{
    const Connection connection = Connection::defaultConnection();
    const QString statement = QueryBuilder::createTableStatement(tableName, blueprint, connection);
    return exec(statement, connection);
}

Result<QSqlQuery, QSqlError> QueryRunner::createTable(const QString &tableName, const TableBlueprint &blueprint, const QString &connectionName)
{
    const Connection connection = Connection::connection(connectionName);
    const QString statement = QueryBuilder::createTableStatement(tableName, blueprint, connection);
    return exec(statement, connection);
}

Result<QSqlQuery, QSqlError> QueryRunner::createTable(const QString &tableName, const TableBlueprint &blueprint, const Connection &connection)
{
    const QString statement = QueryBuilder::createTableStatement(tableName, blueprint, connection);
    return exec(statement, connection);
}

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
