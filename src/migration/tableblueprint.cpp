#include "tableblueprint.h"
#include "tableblueprint_p.h"

namespace QEloquent {

using FieldType = TableBlueprintData::FieldType;

TableBlueprint::TableBlueprint()
    : data(new TableBlueprintData(QString(), false))
{}

TableBlueprint::TableBlueprint(TableBlueprintData *data)
    : data(data)
{}

TableBlueprint::TableBlueprint(const TableBlueprint &rhs)
    : data{rhs.data}
{}

TableBlueprint::TableBlueprint(TableBlueprint &&rhs)
    : data{std::move(rhs.data)}
{}

TableBlueprint &TableBlueprint::operator=(const TableBlueprint &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

TableBlueprint &TableBlueprint::operator=(TableBlueprint &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

TableBlueprint::~TableBlueprint() {}

ColumnDefinition TableBlueprint::id(const QString &name)
{
    return bigInteger(name, true, true).nullable();
}

ColumnDefinition TableBlueprint::uuid(const QString &name)
{
    return character(name, 36).unique();
}

ColumnDefinition TableBlueprint::boolean(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, FieldType::Boolean));
}

ColumnDefinition TableBlueprint::integer(const QString &name, bool autoIncrement, bool positive)
{
    auto d = data->fieldData(name, Column::Type::Integer);
    d->constraints.setFlag(Column::AutoIncrement, autoIncrement);
    d->numberSign = (positive ? Column::NumberSign::Unsigned : Column::NumberSign::Signed);
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::unsignedInteger(const QString &name, bool autoIncrement)
{
    return integer(name, autoIncrement, true);
}

ColumnDefinition TableBlueprint::bigInteger(const QString &name, bool autoIncrement, bool positive)
{
    auto d = data->fieldData(name, Column::Type::BigInteger);
    d->constraints.setFlag(Column::PrimaryKey, autoIncrement);
    d->constraints.setFlag(Column::AutoIncrement, autoIncrement);
    d->numberSign = (positive ? Column::NumberSign::Unsigned : Column::NumberSign::Signed);
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::unsignedBigInteger(const QString &name, bool autoIncrement)
{
    return bigInteger(name, autoIncrement, true);
}

ColumnDefinition TableBlueprint::tinyInteger(const QString &name, bool autoIncrement, bool positive)
{
    auto d = data->fieldData(name, Column::Type::TinyInteger);
    d->constraints.setFlag(Column::AutoIncrement, autoIncrement);
    d->numberSign = (positive ? Column::NumberSign::Unsigned : Column::NumberSign::Signed);
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::unsignedTinyInteger(const QString &name, bool autoIncrement)
{
    return tinyInteger(name, autoIncrement, true);
}

ColumnDefinition TableBlueprint::doubleNumber(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::Type::Double));
}

ColumnDefinition TableBlueprint::floatNumber(const QString &name, int precision)
{
    auto d = data->fieldData(name, Column::Type::Float);
    d->floatPrecision = precision;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::decimal(const QString &name, int total, int places)
{
    auto d = data->fieldData(name, Column::Type::Decimal);
    d->decimalLength = total;
    d->decimalPlaces = places;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::character(const QString &name, int length)
{
    auto d = data->fieldData(name, Column::Type::Char);
    d->length = length;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::string(const QString &name, int length)
{
    auto d = data->fieldData(name, Column::Type::String);
    d->length = length;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::date(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::Type::Date));
}

ColumnDefinition TableBlueprint::time(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::Type::Time));
}

ColumnDefinition TableBlueprint::datetime(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::Type::DateTime));
}

ColumnDefinition TableBlueprint::timestamp(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, FieldType::Timestamp));
}

void TableBlueprint::rawColumn(const QString &name, const QString &definition)
{
    auto d = data->fieldData(name, Column::Type::Raw);
    d->rawDefinition = definition;
}

ForeignKeyDefinition TableBlueprint::foreignId(const QString &name, const QString &tableName, const QString &primaryKey)
{
    auto d = data->fieldData(name, Column::Type::BigInteger);
    d->refTable = tableName;
    d->refColumn = primaryKey;
    return ForeignKeyDefinition(d);
}

void TableBlueprint::timestamps(const QString &creation, const QString &update)
{
    timestamp(creation);
    timestamp(update).nullable();
}

TableBlueprint TableBlueprint::create(const QString &table, bool newTable)
{
    TableBlueprintData *data = new TableBlueprintData(table, !newTable);
    return TableBlueprint(data);
}

ForeignKeyDefinition TableBlueprint::foreignKey(const MetaObject &meta, const QString &fieldName)
{
    const QString tableName = meta.tableName();
    const QString primaryKey = meta.primaryProperty().fieldName();

    if (fieldName.isEmpty())
        return foreignId(meta.foreignProperty().fieldName(), tableName, primaryKey);
    else
        return foreignId(fieldName, tableName, primaryKey);
}

QExplicitlySharedDataPointer<ColumnDefinitionData> &TableBlueprintData::fieldData(const QString &name, FieldType type)
{
    ColumnDefinitionData *data = new ColumnDefinitionData(name, type, tableName);
    columns.append(QExplicitlySharedDataPointer<ColumnDefinitionData>(data));
    return columns.last();
}

ColumnDefinition::ColumnDefinition()
    : data(new ColumnDefinitionData(QString(), Driver::Raw, QString()))
{}


ColumnDefinition::ColumnDefinition(ColumnDefinitionData *data)
    : data(data)
{
}

ColumnDefinition::ColumnDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionData> &data)
    : data(data)
{}

ColumnDefinition::~ColumnDefinition()
{}

ColumnDefinition &ColumnDefinition::primaryKey(bool enable)
{
    data->constraints.setFlag(ColumnDefinitionData::PrimaryKey, enable);
    return *this;
}

ColumnDefinition &ColumnDefinition::autoIncrement()
{
    data->constraints.setFlag(ColumnDefinitionData::PrimaryKey, true);
    data->constraints.setFlag(ColumnDefinitionData::AutoIncrement, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::index()
{
    const QString &name = "idx_" + data->columnName;
    return index(name);
}

ColumnDefinition &ColumnDefinition::index(bool enable)
{
    if (enable) {
        const QString &name = "idx_" + data->columnName;
        return index(name);
    } else {
        return index(QString());
    }
}

ColumnDefinition &ColumnDefinition::index(const QString &name)
{
    if (name.isEmpty()) {
        data->constraints.setFlag(ColumnDefinitionData::Index, false);
    } else {
        data->indexName = name;
        data->constraints.setFlag(ColumnDefinitionData::Index, true);
    }

    return *this;
}

ColumnDefinition &ColumnDefinition::unique(bool enable)
{
    data->constraints.setFlag(ColumnDefinitionData::Unique, enable);
    return *this;
}

ColumnDefinition &ColumnDefinition::nullable(bool enable)
{
    data->constraints.setFlag(ColumnDefinitionData::NotNull, !enable);
    return *this;
}

ColumnDefinition &ColumnDefinition::defaultValue(const QVariant &value)
{
    data->defaultValue = value;
    data->defaultValueIsExpr = false;
    return *this;
}

ColumnDefinition &ColumnDefinition::defaultExpression(const QString &expr)
{
    data->defaultValue = expr;
    data->defaultValueIsExpr = true;
    return *this;
}

ColumnDefinition &ColumnDefinition::min(const QVariant &value)
{
    if (value.isNull()) return *this;
    data->minValue = value;
    data->constraints.setFlag(ColumnDefinitionData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::max(const QVariant &value)
{
    if (value.isNull()) return *this;
    data->maxValue = value;
    data->constraints.setFlag(ColumnDefinitionData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::range(const QVariant &min, const QVariant &max)
{
    if (min.isValid() && max.isValid()) return *this;
    data->minValue = min;
    data->maxValue = max;
    data->constraints.setFlag(ColumnDefinitionData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::check(const QString &expr)
{
    if (expr.isEmpty()) return *this;
    data->constraints.setFlag(ColumnDefinitionData::Check, true);
    data->checkExpr.append(expr);
    return *this;
}

ForeignKeyDefinition::ForeignKeyDefinition()
    : ColumnDefinition(new ColumnDefinitionData(QString(), ColumnDefinitionData::Type::Raw, QString()))
{}


ForeignKeyDefinition::ForeignKeyDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionData> &data)
    : ColumnDefinition(data)
{}

ForeignKeyDefinition::~ForeignKeyDefinition()
{}

ForeignKeyDefinition &ForeignKeyDefinition::onUpdate(ForeignKeyAction action)
{
    data->onUpdate = action;
    return *this;
}

ForeignKeyDefinition &ForeignKeyDefinition::onDelete(ForeignKeyAction action)
{
    data->onDelete = action;
    return *this;
}

} // namespace QEloquent
