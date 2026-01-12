#include "columndata.h"

#include <QEloquent/private/tableblueprint_p.h>

namespace QEloquent {

ColumnData::ColumnData()
    : data(new ColumnDefinitionPrivate())
{}

ColumnData::ColumnData(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &d)
    : data(d)
{}

ColumnData::ColumnData(const ColumnData &other) = default;
ColumnData::ColumnData(ColumnData &&) = default;
ColumnData &ColumnData::operator=(const ColumnData &) = default;
ColumnData &ColumnData::operator=(ColumnData &&) = default;
ColumnData::~ColumnData() = default;

QString ColumnData::columnName() const
{
    return data->columnName;
}

ColumnData::ColumnType ColumnData::columnType() const
{
    return data->type;
}

QString ColumnData::comment() const
{
    return data->comment;
}

ColumnData::NumberSign ColumnData::numberSign() const
{
    return data->numberSign;
}

int ColumnData::decimalPlaces() const
{
    return data->decimalPlaces;
}

int ColumnData::length() const
{
    return data->length;
}

bool ColumnData::hasConstraint(ConstraintFlag constraint) const
{
    return data->constraints.testFlag(constraint);
}

QVariant ColumnData::defaultValue() const
{
    return (data->defaultValueIsExpr ? QVariant() : data->defaultValue);
}

QString ColumnData::defaultExpression(Driver *driver) const
{
    return (data->defaultValueIsExpr ? data->defaultValue.toString() : driver->formatValue(data->defaultValue));
}

QStringList ColumnData::checks(Driver *driver) const
{
    QStringList checks;

    const QString &expr = QStringLiteral("%1 %2= %3")
                              .arg(driver->escapeFieldName(data->columnName));

    if (data->minValue.isValid())
        checks.append(expr.arg(">", driver->formatValue(data->minValue)));

    if (data->maxValue.isValid())
        checks.append(expr.arg("<", driver->formatValue(data->maxValue)));

    if (!data->inValues.isEmpty()) {
        QStringList values;
        std::transform(data->inValues.begin(), data->inValues.end(), std::back_inserter(values), [&driver] (const QVariant &value) {
            return driver->formatValue(value);
        });

        checks.append(driver->escapeFieldName(data->columnName) + " IN (" + values.join(", ") + ')');
    }

    checks.append(data->checkExpr);

    return checks;
}

QString ColumnData::indexName() const
{
    return data->indexName;
}

QString ColumnData::refTable() const
{
    return data->refTable;
}

QStringList ColumnData::refColumns() const
{
    return data->refColumns;
}

ColumnData::ForeignKeyAction ColumnData::onUpdate() const
{
    return data->onUpdate;
}

ColumnData::ForeignKeyAction ColumnData::onDelete() const
{
    return data->onDelete;
}

QString ColumnData::rawDefinition() const
{
    return data->rawDefinition;
}

QString ColumnData::tableName() const
{
    return data->tableName;
}

} // namespace QEloquent
