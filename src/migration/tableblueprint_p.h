#ifndef QELOQUENT_TABLEBLUEPRINT_P_H
#define QELOQUENT_TABLEBLUEPRINT_P_H

#include "tableblueprint.h"

#include <QEloquent/driver.h>

#include <QList>
#include <QVariant>

namespace QEloquent {

class TableBlueprintData final : public QSharedData
{
public:
    using FieldType = Driver::FieldType;

    TableBlueprintData(const QString &table, bool alter)
        : tableName(table), newTable(!alter)
    {}

    const QString tableName;
    QList<QExplicitlySharedDataPointer<ColumnDefinitionData>> columns;
    bool newTable = true;

    QExplicitlySharedDataPointer<ColumnDefinitionData> &fieldData(const QString &name, FieldType type);

    void forEachColumn(const std::function<void(const ColumnDefinitionData &column)> &callback) const {
        std::for_each(columns.begin(), columns.end(), [&callback](const QExplicitlySharedDataPointer<ColumnDefinitionData> &item) {
            callback(*item.constData());
        });
    }
};

class ColumnDefinitionData final : public QSharedData
{
public:
    using Type = Driver::FieldType;

    enum NumberSign {
        Signed = false,
        Unsigned = true
    };

    enum ConstraintFlag {
        NoConstraints = 0x0,
        PrimaryKey = 0x1,
        AutoIncrement = 0x2,
        Unique = 0x4,
        NotNull = 0x8,
        Check = 0x10,
        Index = 0x20,
        ForeignKey = 0x40,
    };
    Q_DECLARE_FLAGS(Constraints, ConstraintFlag)

    using ForeignKeyAction = ForeignKeyDefinition::ForeignKeyAction;

    ColumnDefinitionData(const QString &name, Type t, const QString &table)
        : columnName(name), type(t), tableName(table) {}

    // General data
    QString columnName;
    Type type;
    QString tableName;
    QString comment;

    // Number specifics
    NumberSign numberSign = Signed;
    int decimalLength = 8;
    int decimalPlaces = 2;
    int floatPrecision = 53;

    // String specifics
    QString charset;
    QString collation;
    int length = 255;

    // Constraints
    Constraints constraints = NotNull;
    QString indexName;

    // Foreign
    QString refTable;
    QString refColumn = "id";
    ForeignKeyAction onUpdate = ForeignKeyAction::Restrict;
    ForeignKeyAction onDelete = ForeignKeyAction::Restrict;

    // Default value
    QVariant defaultValue;
    bool defaultValueIsExpr = false;

    // To add to check constraints
    QVariant minValue;
    QVariant maxValue;
    QStringList checkExpr;

    // Raw definition
    QString rawDefinition;

    // Positioning
    QString afterColumn;

    // Helpers
    bool mustInlinePrimaryKey() const { return isPrimaryKey() && isAutoIncrement(); }
    bool isPrimaryKey() const    { return constraints.testFlag(ConstraintFlag::PrimaryKey); }
    bool isAutoIncrement() const { return constraints.testFlag(ConstraintFlag::AutoIncrement); }
    bool isUnique() const        { return constraints.testFlag(ConstraintFlag::Unique); }
    bool isNullable() const      { return !constraints.testFlag(ConstraintFlag::NotNull); }
    bool hasIndex() const        { return constraints.testFlag(ConstraintFlag::Index); }
    bool isForeignKey() const    { return constraints.testFlag(ConstraintFlag::ForeignKey); }
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QEloquent::ColumnDefinitionData::Constraints)

#endif // QELOQUENT_TABLEBLUEPRINT_P_H
