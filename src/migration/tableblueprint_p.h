#ifndef QELOQUENT_TABLEBLUEPRINT_P_H
#define QELOQUENT_TABLEBLUEPRINT_P_H

#include "tableblueprint.h"

#include <QEloquent/columndata.h>
#include <QEloquent/driver.h>

#include <QList>
#include <QVariant>
#include <QSqlRecord>

namespace QEloquent {

class TableBlueprintPrivate final : public QSharedData
{
public:
    using FieldType = ColumnData::ColumnType;

    TableBlueprintPrivate() : newTable(true) {}

    TableBlueprintPrivate(const QString &table, bool create, const Driver *drv)
        : tableName(table), newTable(create), record(create ? QSqlRecord() : drv->record(table)), driver(drv) {}

    const QString tableName;
    QHash<QString, int> columnIndexes;
    QList<QExplicitlySharedDataPointer<ColumnDefinitionPrivate>> columns;

    const bool newTable;
    const QSqlRecord record;
    const Driver *driver;

    QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &fieldData(const QString &name, FieldType type);

    void forEachColumn(const std::function<void(const ColumnDefinitionPrivate &column)> &callback) const {
        std::for_each(columns.begin(), columns.end(), [&callback](const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &item) {
            callback(*item.constData());
        });
    }
};

class ColumnDefinitionPrivate final : public QSharedData
{
public:
    using Type = ColumnData::ColumnType;
    using NumberSign = ColumnData::NumberSign;
    using ConstraintFlag = ColumnData::ConstraintFlag;
    using Constraints = ColumnData::Constraints;
    using ForeignKeyAction = ForeignKeyDefinition::ForeignKeyAction;

    ColumnDefinitionPrivate()
        : ColumnDefinitionPrivate(QString(), Type::Raw, QString()) {}

    ColumnDefinitionPrivate(const QString &name, Type t, const QString &table)
        : columnName(name), type(t), tableName(table) {}

    // General data
    const QString columnName;
    const Type type;
    const QString tableName;
    QString comment;

    // Number specifics
    NumberSign numberSign = NumberSign::Signed;
    int decimalPlaces = 2;
    int floatPrecision = 53;

    // String specifics
    QString charset;
    QString collation;
    int length = 255;

    // Constraints
    Constraints constraints = ConstraintFlag::NotNull;
    QString indexName;

    // Foreign
    QString refTable;
    QStringList refColumns;
    ForeignKeyAction onUpdate = ForeignKeyAction::Restrict;
    ForeignKeyAction onDelete = ForeignKeyAction::Restrict;

    // Default value
    QVariant defaultValue;
    bool defaultValueIsExpr = false;

    // To add to check constraints
    QVariant minValue;
    QVariant maxValue;
    QVariantList inValues;
    QStringList checkExpr;

    // Raw definition
    QString rawDefinition;

    // Positioning
    QString afterColumn;

    // Replacement
    QString oldColumn;
};

}

#endif // QELOQUENT_TABLEBLUEPRINT_P_H
