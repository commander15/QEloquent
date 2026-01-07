#include "schema.h"

#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>
#include <QEloquent/private/tableblueprint_p.h>

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

bool Schema::exists(const QString &tableName)
{
    const Connection conn = connection();
    return conn.database().tables().contains(tableName);
}

void Schema::create(const QString &tableName, const std::function<void (TableBlueprint &)> &callback)
{
    TableBlueprint blueprint = TableBlueprint::create(tableName, true);
    callback(blueprint);

    const QString statement = QueryBuilder::createTableStatement(blueprint, connection());
    exec(statement);
}

void Schema::table(const QString &tableName, const std::function<void (TableBlueprint &)> &callback)
{
    TableBlueprint blueprint = TableBlueprint::create(tableName, false);
    callback(blueprint);

    const QString statement = QueryBuilder::alterTableStatement(blueprint, connection());
    exec(statement);
}

void Schema::drop(const QString &tableName)
{
    const QString statement = "DROP TABLE " + QueryBuilder::escapeTableName(tableName, connection());
    exec(statement);
}

void Schema::dropIfExists(const QString &tableName)
{
    const QString statement = "DROP TABLE IF EXISTS " + QueryBuilder::escapeTableName(tableName, connection());
    exec(statement);
}

QString Schema::connectionName()
{
    if (s_connectionName.isEmpty())
        return Connection::defaultConnectionName();
    else
        return s_connectionName;
}

Connection Schema::connection()
{
    if (s_connectionName.isEmpty())
        return Connection::defaultConnection();
    else
        return Connection::connection(s_connectionName);
}

void Schema::setConnection(const QString &name)
{
    s_connectionName = name;
}

QSqlQuery Schema::exec(const QString &statement)
{
    auto result = QueryRunner::exec(statement, connection());
    if (result) return std::move(result.value());
    throw SchemaException(statement, result.error());
}

QString Schema::s_connectionName;

} // namespace QEloquent
