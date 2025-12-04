#include "modelquery.h"

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

    QList<Filter> filters;
    QStringList groups;
    QList<Sort> sorts;

    int limit = -1;
    int offset = -1;

    QStringList relations;
};

ModelQuery::ModelQuery()
    : data(new ModelQueryData)
{
}

ModelQuery::ModelQuery(const ModelQuery &rhs)
    : data{rhs.data}
{
}

ModelQuery::ModelQuery(ModelQuery &&rhs)
    : data{std::move(rhs.data)}
{
}

ModelQuery &ModelQuery::operator=(const ModelQuery &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

ModelQuery &ModelQuery::operator=(ModelQuery &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

ModelQuery::~ModelQuery()
{
}

ModelQuery &ModelQuery::andWhere(const QString &name, const QString &op, const QVariant &value)
{
    ModelQueryData::Filter f;
    f.inclusive = true;
    f.field = name;
    f.op = op;
    f.value = value;
    data->filters.append(f);
    return *this;
}

ModelQuery &ModelQuery::andWhere(const QString &expression)
{
    ModelQueryData::Filter f;
    f.inclusive = true;
    f.expression = expression;
    data->filters.append(f);
    return *this;
}

ModelQuery &ModelQuery::orWhere(const QString &field, const QString &op, const QVariant &value)
{
    ModelQueryData::Filter f;
    f.inclusive = false;
    f.field = field;
    f.op = op;
    f.value = value;
    data->filters.append(f);
    return *this;
}

ModelQuery &ModelQuery::orWhere(const QString &expression)
{
    ModelQueryData::Filter f;
    f.inclusive = false;
    f.expression = expression;
    data->filters.append(f);
    return *this;
}

ModelQuery &ModelQuery::groupBy(const QString &field)
{
    data->groups.append(field);
    return *this;
}

ModelQuery &ModelQuery::orderBy(const QString &field, Qt::SortOrder order)
{
    ModelQueryData::Sort s;
    s.field = field;
    s.order = order;
    data->sorts.append(s);
    return *this;
}

ModelQuery &ModelQuery::page(int page, int countPerPage)
{
    if (page > 0 && countPerPage > 0) {
        data->limit = countPerPage;
        data->offset = (page - 1) * countPerPage;
    }
    return *this;
}

ModelQuery &ModelQuery::limit(int limit)
{
    data->limit = limit;
    return *this;
}

ModelQuery &ModelQuery::offset(int offset)
{
    data->offset = offset;
    return *this;
}

ModelQuery &ModelQuery::with(const QString &relation)
{
    data->relations.append(relation);
    return *this;
}

ModelQuery &ModelQuery::with(const QStringList &relations)
{
    data->relations.append(relations);
    return *this;
}

bool ModelQuery::hasWhere() const
{
    return !data->filters.isEmpty();
}

QString ModelQuery::whereClause(const Connection &connection) const
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

bool ModelQuery::hasGroupBy() const
{
    return !data->groups.isEmpty();
}

QString ModelQuery::groupByClause(const Connection &connection) const
{
    QStringList groups = data->groups;
    std::transform(groups.begin(), groups.end(), groups.begin(), [connection](const QString &group) {
        return QueryBuilder::escapeFieldName(group, connection);
    });
    return (groups.isEmpty() ? QString() : "GROUP BY " + groups.join(", "));
}

QString ModelQuery::orderByClause(const Connection &connection) const
{
    QStringList sorts;
    std::transform(data->sorts.begin(), data->sorts.end(), std::back_inserter(sorts), [connection](const ModelQueryData::Sort &sort) {
        return QueryBuilder::escapeFieldName(sort.field, connection) + ' ' + (sort.order == Qt::AscendingOrder ? "ASC" : "DESC");
    });
    return (sorts.isEmpty() ? QString() : "ORDER BY " + sorts.join(", "));
}

QString ModelQuery::limitClause() const
{
    return (data->limit > 0 ? "LIMIT " + QString::number(data->limit) : QString());
}

QString ModelQuery::offsetClause() const
{
    return (data->offset > 0 ? "OFFSET " + QString::number(data->offset) : QString());
}

QString ModelQuery::toString(const Connection &connection) const
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

QString ModelQuery::toString() const
{
    return toString(Connection());
}

QStringList ModelQuery::relations() const
{
    return data->relations;
}

} // namespace QEloquent
