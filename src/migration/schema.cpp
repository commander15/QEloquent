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

bool Schema::hasTable(const QString &table)
{
    const Connection conn = connection();
    return conn.database().tables().contains(table);
}

void Schema::create(const QString &table, const BlueprintCallback &callback)
{
    TableBlueprint blueprint = TableBlueprint::create(table, true, connectionName());
    callback(blueprint);

    const QStringList statements = QueryBuilder::createTableStatements(blueprint, connection());
    exec(statements);
}

void Schema::table(const QString &table, const BlueprintCallback &callback)
{
    TableBlueprint blueprint = TableBlueprint::create(table, false, connectionName());
    callback(blueprint);

    const QStringList statements = QueryBuilder::alterTableStatements(blueprint, connection());
    exec(statements);
}

void Schema::rename(const QString &from, const QString &to)
{
    const QStringList statements = grammar()->renameTableStatements(from, to);
    exec(statements);
}

void Schema::drop(const QString &table)
{
    const QStringList statements = grammar()->dropTableStatements(table, false);
    exec(statements);
}

void Schema::dropIfExists(const QString &table)
{
    const QStringList statements = grammar()->dropTableStatements(table, true);
    exec(statements);
}

bool Schema::hasColumn(const QString &column, const QString &table)
{
    return hasColumns(QStringList() << column, table);
}

bool Schema::hasColumns(const QStringList &columns, const QString &table)
{
    const QSqlRecord record = connection().database().record(table);
    for (const QString &column : columns)
        if (!record.contains(column))
            return false;

    return true;
}

void Schema::whenHasColumn(const QString &column, const QString &table, const DefaultCallback &callback)
{
    if (hasColumn(column, table))
        callback();
}

void Schema::whenDoesntHaveColumn(const QString &column, const QString &table, const DefaultCallback &callback)
{
    if (!hasColumn(column, table))
        callback();
}

void Schema::dropColumn(const QString &column, const QString &table)
{
    dropColumns(QStringList() << column, table);
}

void Schema::dropColumns(const QStringList &columns, const QString &table)
{
    const SchemaGrammar *grammar = Schema::grammar();

    QStringList statements;
    for (const QString &column : columns)
        statements.append(grammar->dropColumnStatements(column, table));
    exec(statements);
}

QSqlQuery Schema::exec(const QString &statement)
{
    auto result = QueryRunner::exec(statement, connection());
    if (result) return std::move(result.value());
    else throw SchemaException(statement, result.error());
}

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

SchemaGrammar *Schema::grammar()
{
    const Driver *driver = connection().driver();
    return driver->schemaGrammar();
}

QString Schema::s_connectionName;

} // namespace QEloquent
