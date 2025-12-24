#include "tableblueprint.h"
#include "tableblueprint_p.h"

namespace QEloquent {

using FieldType = TableBlueprintData::FieldType;

TableBlueprint::TableBlueprint()
    : data(new TableBlueprintData)
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

TableFieldBlueprint TableBlueprint::id(const QString &name)
{
    auto d = data->fieldData(name, FieldType::Integer);
    d->primaryKey = true;
    return TableFieldBlueprint(d);
}

TableFieldBlueprint TableBlueprint::boolean(const QString &name)
{
    return TableFieldBlueprint(data->fieldData(name, FieldType::Boolean));
}

TableFieldBlueprint TableBlueprint::integer(const QString &name)
{
    return TableFieldBlueprint(data->fieldData(name, FieldType::Integer));
}

TableFieldBlueprint TableBlueprint::decimal(const QString &name)
{
    return TableFieldBlueprint(data->fieldData(name, FieldType::Double));
}

TableFieldBlueprint TableBlueprint::string(const QString &name)
{
    return TableFieldBlueprint(data->fieldData(name, FieldType::Varchar));
}

TableFieldBlueprint TableBlueprint::timestamp(const QString &name)
{
    return TableFieldBlueprint(data->fieldData(name, FieldType::Timestamp));
}

TableFieldBlueprint TableBlueprint::foreignId(const QString &name, const QString &tableName, const QString &primaryKey)
{
    return TableFieldBlueprint(data->fieldData(name, FieldType::Integer));
}

void TableBlueprint::timestamps(const QString &creation, const QString &update)
{
    timestamp(creation);
    timestamp(update);
}

QExplicitlySharedDataPointer<TableFieldBlueprintData> &TableBlueprintData::fieldData(const QString &name, FieldType type)
{
    QExplicitlySharedDataPointer<TableFieldBlueprintData> data(new TableFieldBlueprintData());
    data->name = name;
    data->type = type;
    fields.append(data);
    return fields.last();
}

TableFieldBlueprint &TableFieldBlueprint::unique()
{
    data->unique = true;
    return *this;
}

} // namespace QEloquent
