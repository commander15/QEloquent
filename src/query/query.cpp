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

    QString tableName;
    QStringList relations;

    QList<Filter> filters;
    QStringList groups;
    QList<Sort> sorts;

    int limit = -1;
    int offset = -1;

    QString connectionName;
};

Query::Query()
    : data(new ModelQueryData)
{
}

Query::Query(const Query &rhs)
    : data{rhs.data}
{
}

Query::Query(Query &&rhs)
    : data{std::move(rhs.data)}
{
}

Query &Query::operator=(const Query &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

Query &Query::operator=(Query &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

Query::~Query()
{
}

QString Query::tableName() const
{
    return data->tableName;
}

Query &Query::table(const QString &tableName)
{
    data->tableName = tableName;
    return *this;
}

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

Query &Query::andWhere(const QString &expression)
{
    ModelQueryData::Filter f;
    f.inclusive = true;
    f.expression = expression;
    data->filters.append(f);
    return *this;
}

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

Query &Query::orWhere(const QString &expression)
{
    ModelQueryData::Filter f;
    f.inclusive = false;
    f.expression = expression;
    data->filters.append(f);
    return *this;
}

Query &Query::groupBy(const QString &field)
{
    data->groups.append(field);
    return *this;
}

Query &Query::orderBy(const QString &field, Qt::SortOrder order)
{
    ModelQueryData::Sort s;
    s.field = field;
    s.order = order;
    data->sorts.append(s);
    return *this;
}

Query &Query::page(int page, int countPerPage)
{
    if (countPerPage > 0)
        limit(countPerPage);

    if (page > 1)
        offset(page - 1);

    return *this;
}

Query &Query::limit(int limit)
{
    data->limit = limit;
    return *this;
}

Query &Query::offset(int offset)
{
    data->offset = offset;
    return *this;
}

Query &Query::with(const QString &relation)
{
    data->relations.append(relation);
    return *this;
}

Query &Query::with(const QStringList &relations)
{
    data->relations.append(relations);
    return *this;
}

Connection Query::connection() const
{
    return Connection::connection(data->connectionName);
}

Query &Query::connection(const QString &connectionName)
{
    data->connectionName = connectionName;
    return *this;
}

bool Query::hasWhere() const
{
    return !data->filters.isEmpty();
}

QString Query::whereClause() const
{
    return whereClause(Connection::connection(data->connectionName));
}

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

bool Query::hasGroupBy() const
{
    return !data->groups.isEmpty();
}

QString Query::groupByClause() const
{
    return groupByClause(Connection::connection(data->connectionName));
}

QString Query::groupByClause(const Connection connection) const
{
    QStringList groups = data->groups;
    std::transform(groups.begin(), groups.end(), groups.begin(), [connection](const QString &group) {
        return QueryBuilder::escapeFieldName(group, connection);
    });
    return (groups.isEmpty() ? QString() : "GROUP BY " + groups.join(", "));
}

bool Query::hasOrderBy() const
{
    return !data->sorts.isEmpty();
}

QString Query::orderByClause() const
{
    return orderByClause(Connection::connection(data->connectionName));
}

QString Query::orderByClause(const Connection connection) const
{
    QStringList sorts;
    std::transform(data->sorts.begin(), data->sorts.end(), std::back_inserter(sorts), [connection](const ModelQueryData::Sort &sort) {
        return QueryBuilder::escapeFieldName(sort.field, connection) + ' ' + (sort.order == Qt::AscendingOrder ? "ASC" : "DESC");
    });
    return (sorts.isEmpty() ? QString() : "ORDER BY " + sorts.join(", "));
}

QString Query::limitClause() const
{
    return (data->limit > 0 ? "LIMIT " + QString::number(data->limit) : QString());
}

QString Query::offsetClause() const
{
    return (data->offset > 0 ? "OFFSET " + QString::number(data->offset) : QString());
}

QString Query::toString() const
{
    return toString(Connection::connection(data->connectionName));
}

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

QStringList Query::relations() const
{
    return data->relations;
}

} // namespace QEloquent
