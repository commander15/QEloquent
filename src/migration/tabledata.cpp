#include "tabledata.h"

#include <QEloquent/columndata.h>
#include <QEloquent/private/tableblueprint_p.h>

namespace QEloquent {

/*!
 * \class TableData
 * \brief Immutable snapshot of a table's structure as defined in a TableBlueprint.
 *
 * TableData provides a read-only view of the complete table definition after
 * the blueprint has been fully configured. It contains the table name, a flag
 * indicating whether the table is new or being altered, and lists of all columns
 * and indexes defined.
 *
 * Instances are typically returned by \c TableBlueprint::table() and passed to
 * SchemaGrammar for SQL generation. They are also useful for introspection or
 * validation after blueprint processing.
 *
 * \note This class is intentionally immutable and lightweight (shared data).
 *       All configuration happens in TableBlueprint or ColumnDefinition.
 *
 * \sa TableBlueprint for the mutable builder.
 * \sa ColumnData for individual column details.
 */

/*!
 * \brief Default constructor.
 *
 * Creates an empty table data object with default values.
 * Internal private data is initialized.
 */
TableData::TableData()
    : data(new TableBlueprintPrivate())
{}

TableData::TableData(const QExplicitlySharedDataPointer<TableBlueprintPrivate> &d)
    : data(d)
{}

/*!
 * \brief Copy constructor.
 *
 * Creates a new table data object as a copy of another instance.
 * This is the default implementation (shared data copy).
 */
TableData::TableData(const TableData &) = default;

/*!
 * \brief Move constructor.
 *
 * Creates a new table data object by moving from another instance.
 * This is the default implementation.
 */
TableData::TableData(TableData &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another table data object to this instance.
 * This is the default implementation.
 *
 * \return Reference to this object.
 */
TableData &TableData::operator=(const TableData &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another table data object into this instance.
 * This is the default implementation.
 *
 * \return Reference to this object.
 */
TableData &TableData::operator=(TableData &&) = default;

/*!
 * \brief Destructor.
 *
 * Destroys this table data instance.
 * This is the default implementation.
 */
TableData::~TableData() = default;

/*!
 * \brief Returns the name of the table.
 *
 * \return The table name as defined in the blueprint.
 */
QString TableData::tableName() const
{
    return data->tableName;
}

/*!
 * \brief Indicates whether this is a new table creation or an alteration.
 *
 * \return \c true if the blueprint was created for a new table (CREATE),
 *         \c false if for altering an existing table (ALTER).
 */
bool TableData::newTable() const
{
    return data->newTable;
}

/*!
 * \brief Returns a list of all column definitions in this table.
 *
 * Converts internal private data into immutable \c ColumnData objects.
 *
 * \return A list of ColumnData objects representing all columns.
 */
QList<ColumnData> TableData::columns() const
{
    QList<ColumnData> columns;
    std::transform(data->columns.begin(), data->columns.end(), std::back_inserter(columns), [](const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &data) {
        return ColumnData(data);
    });
    return columns;
}

/*!
 * \brief Returns all defined indexes grouped by name.
 *
 * Collects and groups columns that share the same index name.
 * Each pair contains the index name and the list of columns it covers.
 *
 * \return List of pairs: (index name, list of column names).
 */
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
