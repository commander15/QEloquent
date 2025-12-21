#include "query.h"

#include <QEloquent/connection.h>
#include <QEloquent/querybuilder.h>

#include <QVariant>
#include <QSqlDriver>
#include <QSqlField>

namespace QEloquent {

class ModelQueryData : public QSharedData
{
public:
    struct Filter {
        bool inclusive = true;
        QString field;
        QString op;
        QVariant value;
        QString expression;
    };

    struct Sort {
        QString field;
        Qt::SortOrder order = Qt::DescendingOrder;
    };

    ModelQueryData() : connectionName(Connection::defaultConnection().name()) {}

    QString tableName;
    QStringList relations;

    QList<Filter> filters;
    QStringList groups;
    QList<Sort> sorts;

    int limit = -1;
    int offset = -1;

    QString connectionName;
};

/*!
 * \class QEloquent::Query
 * \brief Fluent SQL query builder.
 *
 * Query provides a chainable API to build SELECT, INSERT, UPDATE, and DELETE
 * statements without writing raw SQL.
 *
 * \note Query use field names instead of property names.
 */

/*!
 * \brief Constructs an empty query.
 */
Query::Query()
    : data(new ModelQueryData)
{
}

/*!
 * \brief Copy constructor.
 */
Query::Query(const Query &rhs)
    : data{rhs.data}
{
}

/*!
 * \brief Move constructor.
 */
Query::Query(Query &&rhs)
    : data{std::move(rhs.data)}
{
}

/*!
 * \brief Copy assignment operator.
 */
Query &Query::operator=(const Query &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

/*!
 * \brief Move assignment operator.
 */
Query &Query::operator=(Query &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

/*!
 * \brief Destructor.
 */
Query::~Query()
{
}

/*!
 * \brief Returns the table name configured for this query.
 */
QString Query::tableName() const
{
    return data->tableName;
}

/*!
 * \brief Configures the table name for the query.
 */
Query &Query::table(const QString &tableName)
{
    data->tableName = tableName;
    return *this;
}

/*!
 * \brief Adds an AND WHERE clause with a custom operator.
 */
Query &Query::andWhere(const QString &name, const QString &op, const QVariant &value)
{
    ModelQueryData::Filter f;
    f.inclusive = true;
    f.field = name;
    f.op = op;
    f.value = value;
    data->filters.append(f);
    return *this;
}

/*!
 * \brief Adds a raw AND WHERE expression.
 */
Query &Query::andWhere(const QString &expression)
{
    ModelQueryData::Filter f;
    f.inclusive = true;
    f.expression = expression;
    data->filters.append(f);
    return *this;
}

/*!
 * \brief Adds an OR WHERE clause with a custom operator.
 */
Query &Query::orWhere(const QString &field, const QString &op, const QVariant &value)
{
    ModelQueryData::Filter f;
    f.inclusive = false;
    f.field = field;
    f.op = op;
    f.value = value;
    data->filters.append(f);
    return *this;
}

/*!
 * \brief Adds a raw OR WHERE expression.
 */
Query &Query::orWhere(const QString &expression)
{
    ModelQueryData::Filter f;
    f.inclusive = false;
    f.expression = expression;
    data->filters.append(f);
    return *this;
}

/*!
 * \brief Adds a GROUP BY clause.
 */
Query &Query::groupBy(const QString &field)
{
    data->groups.append(field);
    return *this;
}

/*!
 * \brief Adds an ORDER BY clause.
 */
Query &Query::orderBy(const QString &field, Qt::SortOrder order)
{
    ModelQueryData::Sort s;
    s.field = field;
    s.order = order;
    data->sorts.append(s);
    return *this;
}

/*!
 * \brief Configures pagination (LIMIT/OFFSET).
 */
Query &Query::page(int page, int countPerPage)
{
    if (countPerPage > 0)
        limit(countPerPage);

    if (page > 1)
        offset(page - 1);

    return *this;
}

/*!
 * \brief Adds a LIMIT clause.
 */
Query &Query::limit(int limit)
{
    data->limit = limit;
    return *this;
}

/*!
 * \brief Adds an OFFSET clause.
 */
Query &Query::offset(int offset)
{
    data->offset = offset;
    return *this;
}

/*!
 * \brief Adds a relationship for eager loading.
 */
Query &Query::with(const QString &relation)
{
    data->relations.append(relation);
    return *this;
}

/*!
 * \brief Adds multiple relationships for eager loading.
 */
Query &Query::with(const QStringList &relations)
{
    data->relations.append(relations);
    return *this;
}

/*!
 * \brief Returns the connection used by this query.
 */
Connection Query::connection() const
{
    return Connection::connection(data->connectionName);
}

/*!
 * \brief Returns the name of the connection.
 */
QString Query::connectionName() const
{
    return data->connectionName;
}

/*!
 * \brief Configures the connection name for the query.
 */
Query &Query::connection(const QString &connectionName)
{
    data->connectionName = connectionName;
    return *this;
}

/*!
 * \brief Returns true if the query has WHERE clauses.
 */
bool Query::hasWhere() const
{
    return !data->filters.isEmpty();
}

/*!
 * \brief Returns the generated WHERE clause string.
 */
QString Query::whereClause() const
{
    return whereClause(Connection::connection(data->connectionName));
}

/*!
 * \brief Returns the generated WHERE clause string for a specific connection.
 */
QString Query::whereClause(const Connection connection) const
{
    QStringList expressions;

    bool first = true;
    for (const ModelQueryData::Filter &filter : data->filters) {
        QString logicalOperator;
        if (first)
            first = false;
        else
            logicalOperator = (filter.inclusive ? "AND " : "OR ");

        QString expression;
        if (!filter.expression.isEmpty())
            expression = logicalOperator + filter.expression;
        else if (!filter.field.isEmpty()) {
            expression = logicalOperator + QueryBuilder::escapeFieldName(filter.field, connection);
            expression.append(' ' + (filter.op.isEmpty() ? "=" : filter.op));
            expression.append(' ' + (filter.value.isNull() ? "NULL" : QueryBuilder::formatValue(filter.value, connection)));
        }

        if (!expression.isEmpty())
            expressions.append(expression);
    }

    return (expressions.isEmpty() ? QString() : "WHERE " + expressions.join(' '));
}

/*!
 * \brief Returns true if the query has GROUP BY clauses.
 */
bool Query::hasGroupBy() const
{
    return !data->groups.isEmpty();
}

/*!
 * \brief Returns the generated GROUP BY clause string.
 */
QString Query::groupByClause() const
{
    return groupByClause(Connection::connection(data->connectionName));
}

/*!
 * \brief Returns the generated GROUP BY clause string for a specific connection.
 */
QString Query::groupByClause(const Connection connection) const
{
    QStringList groups = data->groups;
    std::transform(groups.begin(), groups.end(), groups.begin(), [connection](const QString &group) {
        return QueryBuilder::escapeFieldName(group, connection);
    });
    return (groups.isEmpty() ? QString() : "GROUP BY " + groups.join(", "));
}

/*!
 * \brief Returns true if the query has ORDER BY clauses.
 */
bool Query::hasOrderBy() const
{
    return !data->sorts.isEmpty();
}

/*!
 * \brief Returns the generated ORDER BY clause string.
 */
QString Query::orderByClause() const
{
    return orderByClause(Connection::connection(data->connectionName));
}

/*!
 * \brief Returns the generated ORDER BY clause string for a specific connection.
 */
QString Query::orderByClause(const Connection connection) const
{
    QStringList sorts;
    std::transform(data->sorts.begin(), data->sorts.end(), std::back_inserter(sorts), [connection](const ModelQueryData::Sort &sort) {
        return QueryBuilder::escapeFieldName(sort.field, connection) + ' ' + (sort.order == Qt::AscendingOrder ? "ASC" : "DESC");
    });
    return (sorts.isEmpty() ? QString() : "ORDER BY " + sorts.join(", "));
}

/*!
 * \brief Returns the generated LIMIT clause string.
 */
QString Query::limitClause() const
{
    return (data->limit > 0 ? "LIMIT " + QString::number(data->limit) : QString());
}

/*!
 * \brief Returns the generated OFFSET clause string.
 */
QString Query::offsetClause() const
{
    return (data->offset > 0 ? "OFFSET " + QString::number(data->offset) : QString());
}

/*!
 * \brief Returns the full SQL statement (SELECT).
 */
QString Query::toString() const
{
    return toString(Connection::connection(data->connectionName));
}

/*!
 * \brief Returns the full SQL statement for a specific connection.
 */
QString Query::toString(const Connection connection) const
{
    QStringList clauses;

    const QString whereClause = this->whereClause(connection);
    if (!whereClause.isEmpty())
        clauses.append(whereClause);

    const QString groupByClause = this->groupByClause(connection);
    if (!groupByClause.isEmpty())
        clauses.append(groupByClause);

    const QString orderByClause = this->orderByClause(connection);
    if (!orderByClause.isEmpty())
        clauses.append(orderByClause);

    const QString limit = this->limitClause();
    if (!limit.isEmpty())
        clauses.append(limit);

    const QString offset = this->offsetClause();
    if (!offset.isEmpty())
        clauses.append(offset);

    return clauses.join(' ');
}

/*!
 * \brief Returns the list of relations to be eager loaded.
 */
QStringList Query::relations() const
{
    return data->relations;
}

} // namespace QEloquent
