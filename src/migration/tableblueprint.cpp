#include "tableblueprint.h"
#include "tableblueprint_p.h"

#include <QEloquent/tabledata.h>
#include <QEloquent/connection.h>
#include <QEloquent/driver.h>
#include <QEloquent/schemagrammar.h>

#include <QSqlField>

namespace QEloquent {

using FieldType = TableBlueprintPrivate::FieldType;

TableBlueprint::TableBlueprint()
    : data(new TableBlueprintPrivate())
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
    return idTyped(name).primaryKey().autoIncrement().nullable();
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
    auto d = data->fieldData(name, Column::ColumnType::Integer);
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
    auto d = data->fieldData(name, Column::ColumnType::BigInteger);
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
    auto d = data->fieldData(name, Column::ColumnType::TinyInteger);
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
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::Double));
}

ColumnDefinition TableBlueprint::floatNumber(const QString &name, int precision)
{
    auto d = data->fieldData(name, Column::ColumnType::Float);
    d->floatPrecision = precision;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::decimal(const QString &name, int total, int places)
{
    auto d = data->fieldData(name, Column::ColumnType::Decimal);
    d->length = total;
    d->decimalPlaces = places;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::character(const QString &name, int length)
{
    auto d = data->fieldData(name, Column::ColumnType::Char);
    d->length = length;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::string(const QString &name, int length)
{
    auto d = data->fieldData(name, Column::ColumnType::String);
    d->length = length;
    return ColumnDefinition(d);
}

ColumnDefinition TableBlueprint::date(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::Date));
}

ColumnDefinition TableBlueprint::time(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::Time));
}

ColumnDefinition TableBlueprint::datetime(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, Column::ColumnType::DateTime));
}

ColumnDefinition TableBlueprint::timestamp(const QString &name)
{
    return ColumnDefinition(data->fieldData(name, FieldType::Timestamp));
}

void TableBlueprint::timestamps(const QString &creation, const QString &update)
{
    timestamp(creation);
    timestamp(update).nullable();
}

ForeignKeyDefinition TableBlueprint::foreignId(const QString &name)
{
    idTyped(name);
    return foreign(name);
}

ForeignKeyDefinition TableBlueprint::foreign(const QString &name)
{
    if (!data->columnIndexes.contains(name))
        return ForeignKeyDefinition();
    return ForeignKeyDefinition(data->fieldData(name, ColumnData::Raw));
}

void TableBlueprint::rawColumn(const QString &name, const QString &definition)
{
    auto d = data->fieldData(name, Column::ColumnType::Raw);
    d->rawDefinition = definition;
}

TableData TableBlueprint::table() const
{
    return TableData(data);
}

TableBlueprint TableBlueprint::create(const QString &table, bool create, const QString &connectionName)
{
    const Connection conn = Connection::connection(connectionName);
    const Driver *driver = conn.driver();

    TableBlueprintPrivate *data = new TableBlueprintPrivate(table, create, driver);
    return TableBlueprint(data);
}

ColumnDefinition TableBlueprint::idTyped(const QString &name)
{
    const SchemaGrammar *grammar = data->driver->schemaGrammar();
    const QPair<ColumnData::ColumnType, ColumnData::NumberSign> type = grammar->primaryKeyType();

    auto d = data->fieldData(name, type.first);
    d->numberSign = type.second;
    return ColumnDefinition(d);
}

TableBlueprint::TableBlueprint(TableBlueprintPrivate *data)
    : data(data)
{}

QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &TableBlueprintPrivate::fieldData(const QString &name, FieldType type)
{
    if (columnIndexes.contains(name))
        return columns[columnIndexes.value(name)];

    ColumnDefinitionPrivate *data = new ColumnDefinitionPrivate(name, type, tableName);
    if (!newTable && record.contains(name)) {
        // Help distinguish new/old field
        data->oldColumn = name;

        const QSqlField field = record.field(name);
        data->constraints.setFlag(ColumnData::NotNull, field.requiredStatus() == QSqlField::Required);
        data->defaultValue = field.defaultValue();
    }

    columns.append(QExplicitlySharedDataPointer<ColumnDefinitionPrivate>(data));
    columnIndexes.insert(name, columns.size() - 1);
    return columns.last();
}

ColumnDefinition::ColumnDefinition()
    : data(new ColumnDefinitionPrivate())
{}


ColumnDefinition::ColumnDefinition(ColumnDefinitionPrivate *data)
    : data(data)
{
}

ColumnDefinition::ColumnDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data)
    : data(data)
{}

ColumnDefinition::~ColumnDefinition()
{}

ColumnDefinition &ColumnDefinition::primaryKey(bool enable)
{
    data->constraints.setFlag(ColumnData::PrimaryKey, enable);
    return *this;
}

ColumnDefinition &ColumnDefinition::autoIncrement()
{
    data->constraints.setFlag(ColumnData::AutoIncrement, true);
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
    data->indexName = name;
    return *this;
}

ColumnDefinition &ColumnDefinition::unique(bool enable)
{
    data->constraints.setFlag(ColumnData::Unique, enable);
    return *this;
}

ColumnDefinition &ColumnDefinition::nullable(bool enable)
{
    data->constraints.setFlag(ColumnData::NotNull, !enable);
    return *this;
}

ColumnDefinition &ColumnDefinition::defaultValue(const QVariant &value)
{
    data->defaultValue = value;
    data->defaultValueIsExpr = false;
    data->constraints.setFlag(ColumnData::Default, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::defaultExpression(const QString &expr)
{
    data->defaultValue = expr;
    data->defaultValueIsExpr = true;
    data->constraints.setFlag(ColumnData::Default, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::min(const QVariant &value)
{
    if (value.isNull()) return *this;
    data->minValue = value;
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::max(const QVariant &value)
{
    if (value.isNull()) return *this;
    data->maxValue = value;
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::range(const QVariant &min, const QVariant &max)
{
    if (min.isValid() && max.isValid()) return *this;
    data->minValue = min;
    data->maxValue = max;
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::in(const QVariantList &values)
{
    data->inValues.append(values);
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

ColumnDefinition &ColumnDefinition::check(const QString &expr)
{
    if (expr.isEmpty()) return *this;
    data->checkExpr.append(expr);
    data->constraints.setFlag(ColumnData::Check, true);
    return *this;
}

ForeignKeyDefinition::ForeignKeyDefinition()
    : ColumnDefinition(new ColumnDefinitionPrivate())
{}


ForeignKeyDefinition::ForeignKeyDefinition(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data)
    : ColumnDefinition(data)
{}

ForeignKeyDefinition::~ForeignKeyDefinition()
{}

ForeignKeyDefinition &ForeignKeyDefinition::on(const QString &table)
{
    data->refTable = table;
    return *this;
}

ForeignKeyDefinition &ForeignKeyDefinition::references(const QString &column)
{
    data->refColumns.append(column);
    return *this;
}

ForeignKeyDefinition &ForeignKeyDefinition::references(const QStringList &columns)
{
    data->refColumns.append(columns);
    return *this;
}

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
