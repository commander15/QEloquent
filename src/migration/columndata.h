#ifndef QELOQUENT_COLUMNDATA_H
#define QELOQUENT_COLUMNDATA_H

#include <QEloquent/global.h>
#include <QEloquent/tableblueprint.h>

namespace QEloquent {

class QELOQUENT_EXPORT ColumnData
{
public:
    enum ColumnType {
        Raw,

        Boolean,

        Integer,
        BigInteger,
        SmallInteger,
        TinyInteger,

        Float,
        Double,
        Decimal,

        Char,
        String,

        Text,
        MediumText,
        LongText,

        Date,
        Time,
        DateTime,
        Timestamp,

        Binary,
        Json,
    };

    enum NumberSign {
        Signed = false,
        Unsigned = true
    };

    enum ConstraintFlag {
        NoConstraints = 0x0,
        PrimaryKey = 0x1,
        AutoIncrement = 0x2,
        Unique = 0x4,
        Default = 0x8,
        NotNull = 0x10,
        Check = 0x20,
        ForeignKey = 0x40,
    };
    Q_DECLARE_FLAGS(Constraints, ConstraintFlag)

    using ForeignKeyAction = ForeignKeyDefinition::ForeignKeyAction;

public:
    ColumnData();
    ColumnData(const ColumnData &other);
    ColumnData(ColumnData &&);
    ColumnData &operator=(const ColumnData &);
    ColumnData &operator=(ColumnData &&);
    ~ColumnData();

    QString columnName() const;
    ColumnType columnType() const;
    QString comment() const;

    NumberSign numberSign() const;
    int decimalPlaces() const;
    int length() const;

    bool hasConstraint(ConstraintFlag constraint) const;

    QVariant defaultValue() const;
    QString defaultExpression(Driver *driver) const;

    QStringList checks(Driver *driver) const;

    QString indexName() const;

    QString refTable() const;
    QStringList refColumns() const;
    ForeignKeyAction onUpdate() const;
    ForeignKeyAction onDelete() const;

    QString rawDefinition() const;

    QString tableName() const;

private:
    ColumnData(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &d);

    QExplicitlySharedDataPointer<ColumnDefinitionPrivate> data;

    friend class TableData;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QEloquent::ColumnData::Constraints)

} // namespace QEloquent

#endif // QELOQUENT_COLUMNDATA_H
