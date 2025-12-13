#include "querybuilder.h"

#include <QEloquent/query.h>
#include <QEloquent/connection.h>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>
#include <QFile>

namespace QEloquent {

QString QueryBuilder::selectStatement(const Query &query)
{
    return selectStatement("*", query);
}

QString QueryBuilder::selectStatement(const QList<QPair<QString, QString>> &fields, const Query &query)
{
    const Connection connection = query.connection();

    QStringList merged;
    std::transform(fields.begin(), fields.end(), std::back_inserter(merged), [&connection](const QPair<QString, QString> &item) {
        if (item.second.isEmpty())
            return escapeFieldName(item.first, connection);
        else
            return item.first + " AS " + escapeFieldName(item.second, connection);
    });
    return selectStatement(merged.join(", "), query);
}

QString QueryBuilder::selectStatement(const QStringList fields, const Query &query)
{
    const Connection connection = query.connection();

    QStringList all = fields;
    for (QString &field : all)
        field = escapeFieldName(field, connection);
    return selectStatement(all.join(", "), query);
}

QString QueryBuilder::selectStatement(const QString fields, const Query &query)
{
    const Connection connection = query.connection();

    QString statement = "SELECT " + fields + " FROM " + escapeTableName(query.tableName(), connection);
    const QString extra = query.toString(connection);
    if (!extra.isEmpty())
        statement.append(' ' + extra);
    return statement;
}

QString QueryBuilder::insertStatement(const QVariantMap &data, const Query &query)
{
    const Connection connection = query.connection();

    QStringList fields = data.keys();
    QStringList values;
    std::for_each(fields.begin(), fields.end(), [&connection, &values, &data](QString &field) {
        values.append(formatValue(data.value(field), connection));
        field = escapeFieldName(field, connection);
    });

    QString statement = "INSERT INTO " + escapeTableName(query.tableName(), connection);
    statement.append(" (" + fields.join(", ") + ") VALUES (" + values.join(", ") + ')');
    return statement;
}

QString QueryBuilder::updateStatement(const QVariantMap &data, const Query &query)
{
    const Connection connection = query.connection();

    const QStringList fields = data.keys();
    QStringList values;
    std::transform(fields.begin(), fields.end(), std::back_inserter(values), [&connection, &data](const QString &field) {
        return escapeFieldName(field, connection) + " = " + formatValue(data.value(field), connection);
    });

    QString statement = "UPDATE " + escapeTableName(query.tableName(), connection);
    statement.append(" SET " + values.join(", "));

    if (query.hasWhere())
        statement.append(' ' + query.whereClause(connection));

    return statement;
}

QString QueryBuilder::deleteStatement(const Query &query)
{
    const Connection connection = query.connection();

    QString statement = "DELETE FROM " + escapeTableName(query.tableName(), connection);

    if (query.hasWhere())
        statement.append(' ' + query.whereClause(connection));

    return statement;
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

QStringList QueryBuilder::statementsFromScriptFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        return statementsFromScriptContent(file.readAll());
    else
        return QStringList();
}

QStringList QueryBuilder::statementsFromScriptDevice(QIODevice *device)
{
    return statementsFromScriptContent(device->readAll());
}

QStringList QueryBuilder::statementsFromScriptContent(const QByteArray &content)
{
    QStringList statements;
    QStringList input = QString::fromUtf8(content.trimmed()).split('\n');

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

        // We append reading to the current statement
        statement.append(line);

        // If delimiter found, we complete the current statement and move to the next
        if (line.endsWith(delimiter)) {
            statement.removeLast(); // We remove delimiter
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
