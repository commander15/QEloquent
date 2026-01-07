#include "querybuilder.h"

#include <QEloquent/query.h>
#include <QEloquent/connection.h>
#include <QEloquent/datamap.h>
#include <QEloquent/driver.h>
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
#   include <QEloquent/tableblueprint.h>
#   include <QEloquent/private/tableblueprint_p.h>
#endif

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
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

QString QueryBuilder::insertStatement(const DataMap &data, const Query &query)
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

QString QueryBuilder::updateStatement(const DataMap &data, const Query &query)
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

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

QString QueryBuilder::createTableStatement(const TableBlueprint &blueprint, const Connection &connection)
{
    const TableBlueprintData *table = blueprint.data.get();
    if (!table->newTable)
        return QString();

    QStringList fields;
    QStringList constraints;
    table->forEachColumn([&fields, &constraints, &connection](const ColumnDefinitionData &column) {
        fields.append(columnDefinition(column, connection));
        constraints.append(constraintDefinitions(column, connection));
    });

    return QStringLiteral("CREATE TABLE %1 (%2)")
        .arg(escapeTableName(table->tableName, connection), (fields + constraints).join(", "));
}

QString QueryBuilder::alterTableStatement(const TableBlueprint &blueprint, const Connection &connection)
{
    const TableBlueprintData *table = blueprint.data.get();
    QSqlRecord record = connection.database().record(table->tableName);

    QStringList fields;
    table->forEachColumn([&fields, &record, &connection](const ColumnDefinitionData &column) {
        QString prefix;
        if (record.contains(column.columnName)) {
            prefix = "CHANGE";
            record.remove(record.indexOf(column.columnName));
        } else {
            prefix = "ADD COLUMN";
        }

        fields.append(prefix + ' ' + columnDefinition(column, connection));
    });

    for (int i(0); i < record.count(); ++i) {
        fields.append("DROP COLUMN " + escapeFieldName(record.fieldName(i), connection));
    }

    return QStringLiteral("ALTER TABLE %1")
        .arg(escapeTableName(table->tableName, connection), fields.join(" "));
}

#endif

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

QString QueryBuilder::columnDefinition(const ColumnDefinitionData &column, const Connection &connection)
{
    Driver *driver = connection.driver();

    using Constraint = ColumnDefinitionData::ConstraintFlag;

    const QString fieldName = escapeFieldName(column.columnName, connection);

    QString type;
    switch (column.type) {
    case Driver::Float:
    case Driver::Double:
    case Driver::Decimal:
        type = driver->columnType(column.type, column.decimalLength, column.decimalPlaces);
        break;

    case Driver::Char:
    case Driver::String:
        type = driver->columnType(column.type, column.length);
        break;

    default:
        type = driver->columnType(column.type, -1, -1);
        break;
    }

    QStringList constraints;

    if (column.mustInlinePrimaryKey())
        constraints.append("PRIMARY KEY " + driver->autoIncrementKeyword());

    if (!column.isNullable())
        constraints.append("NOT NULL");

    // 'DEFAULT' constraint
    if (column.defaultValue.isValid()) {
        if (column.defaultValueIsExpr)
            constraints.append("DEFAULT " + column.defaultValue.toString());
        else
            constraints.append("DEFAULT " + formatValue(column.defaultValue, connection));
    }

    return fieldName + ' ' + type + (constraints.isEmpty() ? "" : ' ' + constraints.join(' '));
}

QStringList QueryBuilder::constraintDefinitions(const ColumnDefinitionData &column, const Connection &connection)
{
    using Constraint = ColumnDefinitionData::ConstraintFlag;

    if (column.constraints == Constraint::NoConstraints)
        return QStringList();

    QString checkExpr;
    if (column.minValue.isValid() || column.maxValue.isValid() || !column.checkExpr.isEmpty()) {
        const QString fieldName = escapeFieldName(column.columnName, connection);

        QStringList checks;

        if (column.minValue.isValid())
            checks.append(fieldName + " >= " + formatValue(column.minValue, connection));

        if (column.maxValue.isValid())
            checks.append(fieldName + " <= " + formatValue(column.maxValue, connection));

        if (!column.checkExpr.isEmpty())
            checks.append(column.checkExpr);

        checkExpr = checks.join(" AND ");
    }

    QMap<Constraint, QString> standardConstraintDefinitions = {
        {
         Constraint::PrimaryKey,
         QStringLiteral("CONSTRAINT pk_%1")
            .arg(escapeTableName("table", connection))
        },
        {
         Constraint::ForeignKey,
         QStringLiteral("CONSTRAINT fk_%1_%2 FOREIGN KEY(%3) REFERENCES %4(%5)")
             .arg("table", column.columnName,
                  escapeFieldName(column.columnName, connection),
                  escapeTableName(column.refTable, connection),
                  escapeFieldName(column.refColumn, connection))
        },
        {
         Constraint::Unique,
         QStringLiteral("CONSTRAINT un_%1_%2 UNIQUE(%3)")
             .arg("table", column.columnName, escapeFieldName(column.columnName, connection))
        },
        {
         Constraint::Check,
         QStringLiteral("CONSTRAINT ck_%1_%2 CHECK(%3)")
             .arg(column.tableName, column.columnName, checkExpr)
        },
        {
         Constraint::Index,
         QStringLiteral("CREATE INDEX %1 ON %2(%3)")
             .arg(column.indexName,
                  escapeTableName("table", connection),
                  escapeFieldName(column.columnName, connection))
        },
    };

    if (column.mustInlinePrimaryKey())
        standardConstraintDefinitions.remove(Constraint::PrimaryKey);

    QStringList constraints;
    const QList<Constraint> standardConstraints = standardConstraintDefinitions.keys();
    for (Constraint constraint : standardConstraints)
        if (column.constraints.testFlag(constraint))
            constraints.append(standardConstraintDefinitions.value(constraint));

    return constraints;
}

} // namespace QEloquent
