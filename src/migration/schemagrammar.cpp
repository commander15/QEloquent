#include "schemagrammar.h"
#include "schemagrammar_p.h"

#include <QEloquent/driver.h>
#include <QEloquent/tabledata.h>
#include <QEloquent/columndata.h>

#include <QSqlRecord>

namespace QEloquent {

ColumnData::Constraints SchemaGrammar::inlineConstraints() const
{
    return ColumnData::Default | ColumnData::NotNull;
}

ColumnData::Constraints SchemaGrammar::namedConstraints() const
{
    return ColumnData::PrimaryKey | ColumnData::Unique | ColumnData::Check | ColumnData::ForeignKey;
}

QStringList SchemaGrammar::createTableStatements(const TableBlueprint &blueprint) const
{
    const TableData &table = blueprint.table();
    QStringList statements;

    {
        QStringList fields;
        QStringList constraints;

        // We create columns
        const QList<ColumnData> columns = table.columns();
        for (const ColumnData &column : columns) {
            fields.append(columnDefinition(column, true));

            static const ColumnData::Constraints outOfLineConstraints = namedConstraints();
            constraints.append(constraintsDefinitions(column, outOfLineConstraints, true));
        }

        const QString main = QStringLiteral("CREATE TABLE %1 (%2)")
                                 .arg(driver->escapeTableName(table.tableName()), (fields + constraints).join(", "));
        statements.append(main);
    }

    // We create indexes for the table
    const QList<QPair<QString, QStringList>> indexes = table.indexes();
    for (const QPair<QString, QStringList> &index : indexes) {
        const QStringList indexCreation = createIndexStatements(index.first, index.second, table.tableName(), false);
        statements.append(indexCreation);
    }

    return statements;
}

QStringList SchemaGrammar::alterTableStatements(const TableBlueprint &blueprint) const
{
    const TableData &table = blueprint.table();
    QStringList statements;

    // ...

    return statements;
}

QStringList SchemaGrammar::renameTableStatements(const QString &from, const QString &to) const
{
    return {
        QStringLiteral("ALTER TABLE %1 RENAME TO %2")
            .arg(driver->escapeTableName(from), driver->escapeTableName(to))
    };
}

QStringList SchemaGrammar::dropTableStatements(const QString &table, bool ifExists) const
{
    if (ifExists)
        return { QStringLiteral("DROP TABLE IF EXISTS %1").arg(driver->escapeTableName(table)) };
    else
        return { QStringLiteral("DROP TABLE %1").arg(driver->escapeTableName(table)) };
}

QString SchemaGrammar::columnDefinition(const ColumnData &column, bool full) const
{
    QString type;
    switch (column.columnType()) {
    case ColumnType::Raw:
        return column.rawDefinition();

    case ColumnType::Float:
    case ColumnType::Double:
    case ColumnType::Decimal:
        type = columnType(column.columnType(), column.length(), column.decimalPlaces());
        break;

    case ColumnType::Char:
    case ColumnType::String:
        type = columnType(column.columnType(), column.length(), -1);
        break;

    default:
        type = columnType(column.columnType(), -1, -1);
        break;
    }

    if (hasFeature(NumberSignSupport) && column.numberSign() == NumberSign::Unsigned) {
        switch (column.columnType()) {
        case ColumnData::Integer:
        case ColumnData::BigInteger:
        case ColumnData::SmallInteger:
        case ColumnData::TinyInteger:
        case ColumnData::Float:
        case ColumnData::Double:
        case ColumnData::Decimal:
            type.prepend(QStringLiteral("UNSIGNED "));
            break;

        default:
            break;
        }
    }

    QStringList constraints;
    if (full) {
        constraints.append(constraintsDefinitions(column, inlineConstraints(), false));
    }

    const QString fieldName = driver->escapeFieldName(column.columnName());
    return fieldName + ' ' + type + (constraints.isEmpty() ? "" : ' ' + constraints.join(' '));
}

QString SchemaGrammar::columnType(ColumnType type, int length, int precision) const
{
    switch (type) {
    case ColumnType::Boolean:
        return QStringLiteral("BOOLEAN");

    case ColumnType::Integer:
        return "INTEGER";

    case ColumnType::BigInteger:
        return "BIGINT";

    case ColumnType::SmallInteger:
        return "SMALLINT";

    case ColumnType::TinyInteger:
        return "TINYINT";

    case ColumnType::Float:
        return "FLOAT";

    case ColumnType::Double:
        return "DOUBLE";

    case ColumnType::Decimal:
        return QStringLiteral("DECIMAL(%1, %2)").arg(length).arg(precision);

    case ColumnType::Char:
        return QStringLiteral("CHAR(%1)").arg(length);

    case ColumnType::String:
        return QStringLiteral("VARCHAR(%1)").arg(length);

    case ColumnType::Text:
        return "TEXT";

    case ColumnType::MediumText:
        return "MEDIUMTEXT";

    case ColumnType::LongText:
        return "LONGTEXT";

    case ColumnType::Date:
        return "DATE";

    case ColumnType::Time:
        return "TIME";

    case ColumnType::DateTime:
        return "DATETIME";

    case ColumnType::Timestamp:
        return "TIMESTAMP";

    case ColumnType::Binary:
        return "BLOB";

    case ColumnType::Json:
        return "JSON";

    case ColumnType::Raw:
        QString();
    }

    return QString();
}

QStringList SchemaGrammar::constraintsDefinitions(const ColumnData &column, ColumnData::Constraints supported, bool named) const
{
    auto canProcess = [&column, &supported](ColumnData::ConstraintFlag flag) {
        return (supported.testFlag(flag) && column.hasConstraint(flag));
    };

    QStringList constraints;

    // ── PRIMARY KEY ───────────────────────────────────────────────────────
    if (canProcess(ColumnData::PrimaryKey)) {
        if (named) {
            QString pkDef = QStringLiteral("CONSTRAINT pk_%1 PRIMARY KEY(%2%3)")
                                .arg(column.tableName(),
                                     driver->escapeFieldName(column.columnName()),
                                     column.hasConstraint(ColumnData::AutoIncrement)
                                         ? ' ' + autoIncrementKeyword() : QString());
            constraints.append(pkDef);
        } else {
            constraints.append("PRIMARY KEY" + (column.hasConstraint(ColumnData::AutoIncrement) ? ' ' + autoIncrementKeyword() : ""));
        }
    }

    // ── UNIQUE ─────────────────────────────────────────────────────────────
    if (canProcess(ColumnData::Unique)) {
        if (named) {
            QString uniqueDef = QStringLiteral("CONSTRAINT un_%1_%2 UNIQUE(%3)")
                                    .arg(column.tableName(), column.columnName(),
                                         driver->escapeFieldName(column.columnName()));
            constraints.append(uniqueDef);
        } else {
            constraints.append("UNIQUE");
        }
    }

    // ── DEFAULT ─────────────────────────────────────────────────────────────
    if (canProcess(ColumnData::Default) && !named) {
        constraints.append("DEFAULT " + column.defaultExpression(driver));
    }

    // ── NOT NULL ─────────────────────────────────────────────────────────────
    if (canProcess(ColumnData::NotNull) && !named) {
        constraints.append("NOT NULL");
    }

    // ── CHECK ──────────────────────────────────────────────────────────────
    if (canProcess(ColumnData::Check)) {
        QString checkExpr = column.checks(driver).join(" AND ");

        if (named) {
            QString checkDef = QStringLiteral("CONSTRAINT ck_%1_%2 CHECK(%3)")
                                   .arg(column.tableName(),
                                        column.columnName(),
                                        checkExpr);
            constraints.append(checkDef);
        } else {
            constraints.append("CHECK (" + checkExpr + ')');
        }
    }

    // ── FOREIGN KEY ────────────────────────────────────────────────────────
    if (canProcess(ColumnData::ForeignKey)) {
        static const auto actionText = [](ColumnData::ForeignKeyAction action) -> QString {
            switch (action) {
            case ColumnData::ForeignKeyAction::NoAction:   return QStringLiteral("NO ACTION");
            case ColumnData::ForeignKeyAction::Restrict:   return QStringLiteral("RESTRICT");
            case ColumnData::ForeignKeyAction::Cascade:    return QStringLiteral("CASCADE");
            case ColumnData::ForeignKeyAction::SetNull:    return QStringLiteral("SET NULL");
            case ColumnData::ForeignKeyAction::SetDefault: return QStringLiteral("SET DEFAULT");
            default:                                       return QStringLiteral("NO ACTION");
            }
        };

        QStringList escapedRefCols = column.refColumns();
        std::transform(escapedRefCols.begin(), escapedRefCols.end(), escapedRefCols.begin(),
                       [this](const QString &f) { return driver->escapeFieldName(f); });

        QString fkDef = QStringLiteral("FOREIGN KEY(%1) REFERENCES %2(%3)"
                                       " ON UPDATE %4 ON DELETE %5")
                            .arg(driver->escapeFieldName(column.columnName()),
                                 driver->escapeTableName(column.refTable()),
                                 escapedRefCols.join(", "),
                                 actionText(column.onUpdate()),
                                 actionText(column.onDelete()));

        if (named) {
            QString name = QStringLiteral("fk_%1_%2")
                               .arg(column.tableName(), column.columnName());
            constraints.append(QStringLiteral("CONSTRAINT %1 %2").arg(name, fkDef));
        } else {
            constraints.append(fkDef);
        }
    }

    return constraints;
}

QStringList SchemaGrammar::addColumnStatements(const ColumnData &column) const
{
    const QString main = QStringLiteral("ALTER TABLE %1 ADD COLUMN %2")
        .arg(driver->escapeTableName(column.tableName()), columnDefinition(column, hasFeature(FullAlterColumnDefinitionSupport)));

    return QStringList() << main;
}

QStringList SchemaGrammar::renameColumnStatements(const QString &from, const QString &to, const QString &table) const
{
    return {
        QStringLiteral("ALTER TABLE %1 RENAME %2 TO %3")
            .arg(driver->escapeTableName(table),
                 driver->escapeFieldName(from),
                 driver->escapeFieldName(to))
    };
}

QStringList SchemaGrammar::dropColumnStatements(const QString &column, const QString &table) const
{
    return { QStringLiteral("ALTER TABLE %1 DROP COLUMN %2").arg(driver->escapeTableName(table), driver->escapeFieldName(column)) };
}

QStringList SchemaGrammar::createIndexStatements(const QString &index, const QStringList &columns, const QString &table, bool unique) const
{
    QStringList fields;
    std::transform(columns.begin(), columns.end(), std::back_inserter(fields), [this](const QString &column) {
        return driver->escapeFieldName(column);
    });

    if (unique) {
        return {
            QStringLiteral("CREATE UNIQUE INDEX %1 ON %2(%3)")
            .arg(index, driver->escapeTableName(table), fields.join(", "))
        };
    }

    return {
        QStringLiteral("CREATE INDEX %1 ON %2(%3)")
            .arg(index, driver->escapeTableName(table), fields.join(", "))
    };
}

QStringList SchemaGrammar::dropIndexStatements(const QString &index, const QString &table) const
{
    return { QStringLiteral("DROP INDEX %1").arg(index) };
}

SchemaGrammar *SchemaGrammar::createDefault(Driver *driver)
{
    return new DefaultSchemaGrammar(driver);
}

QString SchemaGrammar::fieldName(const ColumnData &column, bool escaped) const
{
    if (escaped)
        return driver->escapeFieldName(column.columnName());
    else
        return column.columnName();
}

QString SchemaGrammar::tableName(const ColumnData &column, bool escaped) const
{
    if (escaped)
        return driver->escapeTableName(column.tableName());
    else
        return column.tableName();
}

} // namespace QEloquent
