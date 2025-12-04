#include "querybuilder.h"

#include <QEloquent/modelquery.h>
#include <QEloquent/modelinfo.h>
#include <QEloquent/connection.h>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>

namespace QEloquent {

QString QueryBuilder::selectStatement(const ModelQuery &query, const ModelInfo &info)
{
    const Connection connection = info.connection();

    QString statement = "SELECT * FROM " + escapeTableName(info.table(), connection);
    const QString extra = query.toString(connection);
    if (!extra.isEmpty())
        statement.append(' ' + extra);
    return statement;
}

QString QueryBuilder::insertStatement(const QVariantMap &data, const ModelInfo &info)
{
    const Connection connection = info.connection();

    QStringList fields = data.keys();
    QStringList values;
    std::for_each(fields.begin(), fields.end(), [&connection, &values, &data](QString &field) {
        values.append(formatValue(data.value(field), connection));
        field = escapeFieldName(field, connection);
    });

    QString statement = "INSERT INTO " + escapeTableName(info.table(), connection);
    statement.append(" (" + fields.join(", ") + ") VALUES (" + values.join(", ") + ')');
    return statement;
}

QString QueryBuilder::updateStatement(const ModelQuery &query, const QVariantMap &data, const ModelInfo &info)
{
    const Connection connection = info.connection();

    QStringList fields = data.keys();
    QStringList values;
    std::for_each(fields.begin(), fields.end(), [&connection, &values, &data](const QString &field) {
        values.append(escapeFieldName(field, connection) + " = " + formatValue(data.value(field), connection));
    });

    QString statement = "UPDATE " + escapeTableName(info.table(), connection);
    statement.append(" SET " + values.join(", "));

    if (query.hasWhere())
        statement.append(' ' + query.whereClause(connection));

    return statement;
}

QString QueryBuilder::deleteStatement(const ModelQuery &query, const ModelInfo &info)
{
    const Connection connection = info.connection();

    QString statement = "DELETE FROM " + escapeTableName(info.table(), connection);

    if (query.hasWhere())
        statement.append(' ' + query.whereClause(connection));

    return statement;
}

QString QueryBuilder::whereClause(const ModelQuery &query, const Connection &connection)
{
    return query.whereClause(connection);
}

QString QueryBuilder::singularise(const QString &tableName)
{
    if (tableName.endsWith("ies"))
        return tableName.left(tableName.size() - 3);

    if (tableName.endsWith("s"))
        return tableName.left(tableName.size() - 1);

    return tableName;
}

QString QueryBuilder::escapeFieldName(const QString &name, const Connection &connection)
{
    QSqlDriver *driver = connection.database().driver();

    if (name.contains('.')) {
        QStringList parts = name.split(".", Qt::SkipEmptyParts);
        if (parts.isEmpty()) return QString();
        else if (parts.size() == 1) return driver->escapeIdentifier(parts.first(), QSqlDriver::FieldName);
        else {
            const QString table = driver->escapeIdentifier(parts.first(), QSqlDriver::TableName);
            const QString field = driver->escapeIdentifier(parts.last(), QSqlDriver::FieldName);
            return table + '.' + field;
        }
    } else {
        return driver->escapeIdentifier(name, QSqlDriver::FieldName);
    }
}

QString QueryBuilder::escapeTableName(const QString &name, const Connection &connection)
{
    return connection.database().driver()->escapeIdentifier(name, QSqlDriver::TableName);
}

QString QueryBuilder::formatValue(const QVariant &value, const Connection &connection)
{
    return formatValue(value, value.metaType(), connection);
}

QString QueryBuilder::formatValue(const QVariant &value, const QMetaType &type, const Connection &connection)
{
    QSqlField field(QString(), type);
    field.setValue(value);
    return connection.database().driver()->formatValue(field);
}

QStringList QueryBuilder::statementsFromScript(const QString &script)
{
    QStringList statements;
    QStringList input = script.trimmed().split('\n');

    QString statement;
    QString delimiter = ";";
    for (QString &line : input) {
        line = line.trimmed();

        // We skip empty lines
        if (line.isEmpty())
            continue;

        // We skip comments
        if (line.startsWith("--"))
            continue;

        // We change delimiter
        if (line.contains("DELIMITER", Qt::CaseInsensitive)) {
            line.remove("DELIMITER", Qt::CaseInsensitive);
            delimiter = line.trimmed();
        }

        statement.append(line);

        if (line.endsWith(delimiter)) {
            statements.append(statement);
            statement.clear();
        }
    }

    // If there is only one statement without delimiter
    if (statements.isEmpty() && !statement.isEmpty())
        statements.append(statement);

    return statements;
}



} // namespace QEloquent
