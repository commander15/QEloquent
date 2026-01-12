#include "tabledata.h"

#include <QEloquent/columndata.h>
#include <QEloquent/private/tableblueprint_p.h>

namespace QEloquent {

TableData::TableData()
    : data(new TableBlueprintPrivate())
{}

TableData::TableData(const QExplicitlySharedDataPointer<TableBlueprintPrivate> &d)
    : data(d)
{}

TableData::TableData(const TableData &) = default;
TableData::TableData(TableData &&) = default;
TableData &TableData::operator=(const TableData &) = default;
TableData &TableData::operator=(TableData &&) = default;
TableData::~TableData() = default;

QString TableData::tableName() const
{
    return data->tableName;
}

bool TableData::newTable() const
{
    return data->newTable;
}

QList<ColumnData> TableData::columns() const
{
    QList<ColumnData> columns;
    std::transform(data->columns.begin(), data->columns.end(), std::back_inserter(columns), [](const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data) {
        return ColumnData(data);
    });
    return columns;
}

QList<QPair<QString, QStringList>> TableData::indexes() const
{
    QMap<QString, int> names;
    QList<QPair<QString, QStringList>> indexes;

    for (auto column : std::as_const(data->columns)) {
        if (column->indexName.isEmpty())
            continue;

        const QString indexName = column->indexName;
        if (names.contains(indexName)) {
            indexes[names.value(indexName)].second.append(column->columnName);
        } else {
            names.insert(indexName, indexes.size());
            QPair<QString, QStringList> i = { indexName, { column->columnName } };
            indexes.append(i);
        }
    }

    return indexes;
}

} // namespace QEloquent
