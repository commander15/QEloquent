#include "querybuilder.h"

#include <QEloquent/query.h>
#include <QEloquent/connection.h>
#include <QEloquent/driver.h>
#include <QEloquent/datamap.h>
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
#   include <QEloquent/schemagrammar.h>
#endif

#include <QFile>

namespace QEloquent {

QString QueryBuilder::selectStatement(const Query &query)
{
    return selectStatement(QStringLiteral("*"), query);
}

QString QueryBuilder::selectStatement(const QList<QPair<QString, QString>> &fields, const Query &query)
{
    const Driver *driver = query.connection().driver();
    if (driver == nullptr) return QString();

    QStringList merged;
    std::transform(fields.begin(), fields.end(), std::back_inserter(merged), [&driver](const QPair<QString, QString> &item) {
        if (item.second.isEmpty())
            return driver->escapeFieldName(item.first);
        else
            return item.first + " AS " + driver->escapeFieldName(item.second);
    });
    return selectStatement(merged.join(", "), query);
}

QString QueryBuilder::selectStatement(const QStringList fields, const Query &query)
{
    const Driver *driver = query.connection().driver();
    if (driver == nullptr) return QString();

    QStringList escaped;
    std::transform(fields.begin(), fields.end(), std::back_inserter(escaped), [&driver](const QString &field) {
        return driver->escapeFieldName(field);
    });
    return selectStatement(escaped.join(", "), query);
}

QString QueryBuilder::selectStatement(const QString fields, const Query &query)
{
    const Driver *driver = query.connection().driver();
    if (driver == nullptr) return QString();

    QString statement = "SELECT " + fields + " FROM " + driver->escapeTableName(query.tableName());
    const QString extra = query.toString(driver);
    if (!extra.isEmpty())
        statement.append(' ' + extra);
    return statement;
}

QString QueryBuilder::insertStatement(const DataMap &data, const Query &query)
{
    const Driver *driver = query.connection().driver();
    if (driver == nullptr) return QString();

    QStringList fields = data.keys();
    QStringList values;
    std::for_each(fields.begin(), fields.end(), [&driver, &values, &data](QString &field) {
        values.append(driver->formatValue(data.value(field)));
        field = driver->escapeFieldName(field);
    });

    QString statement = "INSERT INTO " + driver->escapeTableName(query.tableName());
    statement.append(" (" + fields.join(", ") + ") VALUES (" + values.join(", ") + ')');
    return statement;
}

QString QueryBuilder::updateStatement(const DataMap &data, const Query &query)
{
    const Driver *driver = query.connection().driver();
    if (driver == nullptr) return QString();


    const QStringList fields = data.keys();
    QStringList values;
    std::transform(fields.begin(), fields.end(), std::back_inserter(values), [&driver, &data](const QString &field) {
        return driver->escapeFieldName(field) + " = " + driver->formatValue(data.value(field));
    });

    QString statement = "UPDATE " + driver->escapeTableName(query.tableName());
    statement.append(" SET " + values.join(", "));

    if (query.hasWhere())
        statement.append(' ' + query.whereClause(driver));

    return statement;
}

QString QueryBuilder::deleteStatement(const Query &query)
{
    const Driver *driver = query.connection().driver();
    if (driver == nullptr) return QString();

    QString statement = "DELETE FROM " + driver->escapeTableName(query.tableName());
    if (query.hasWhere())
        statement.append(' ' + query.whereClause(driver));
    return statement;
}

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

QStringList QueryBuilder::createTableStatements(const TableBlueprint &blueprint, const Connection &connection)
{
    const Driver *driver = connection.driver();
    if (driver == nullptr) return QStringList();
    return driver->schemaGrammar()->createTableStatements(blueprint);
}

QStringList QueryBuilder::alterTableStatements(const TableBlueprint &blueprint, const Connection &connection)
{
    const Driver *driver = connection.driver();
    if (driver == nullptr) return QStringList();
    return driver->schemaGrammar()->alterTableStatements(blueprint);
}

#endif

QString QueryBuilder::escapeFieldName(const QString &name, const Connection &connection)
{
    const Driver *driver = connection.driver();
    return (driver == nullptr ? name : driver->escapeFieldName(name));
}

QString QueryBuilder::escapeTableName(const QString &name, const Connection &connection)
{
    const Driver *driver = connection.driver();
    return (driver == nullptr ? name : driver->escapeTableName(name));
}

QString QueryBuilder::formatValue(const QVariant &value, const Connection &connection)
{
    const Driver *driver = connection.driver();
    return (driver == nullptr ? value.toString() : driver->formatValue(value));
}

QString QueryBuilder::formatValue(const QVariant &value, const QMetaType &type, const Connection &connection)
{
    QVariant val(value);
    val.convert(type);

    const Driver *driver = connection.driver();
    return (driver == nullptr ? value.toString() : driver->formatValue(val));
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
    if (device->isReadable())
        return statementsFromScriptContent(device->readAll());
    else
        return QStringList();
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
            statement.remove(statement.size() - 1, 1); // We remove delimiter
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
