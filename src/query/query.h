#ifndef QELOQUENT_QUERY_H
#define QELOQUENT_QUERY_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

namespace QEloquent {

class Connection;

class ModelQueryData;
/**
 * @brief Fluent SQL query builder.
 * 
 * Query provides a chainable API to build SELECT, INSERT, UPDATE, and DELETE 
 * statements without writing raw SQL.
 */
class Query
{
public:
    /** @brief Constructs an empty query */
    Query();
    /** @brief Copy constructor */
    Query(const Query &);
    /** @brief Move constructor */
    Query(Query &&);
    /** @brief Copy assignment operator */
    Query &operator=(const Query &);
    /** @brief Move assignment operator */
    Query &operator=(Query &&);
    /** @brief Destructor */
    ~Query();

    /** @brief Returns the table name configured for this query */
    QString tableName() const;
    /** @brief Configures the table name for the query */
    Query &table(const QString &tableName);

    /** @brief Adds a simple WHERE equal clause */
    Query &where(const QString &field, const QVariant &value) { return andWhere(field, "=", value); }
    /** @brief Adds a WHERE clause with a custom operator */
    Query &where(const QString &field, const QString &op, const QVariant &value) { return andWhere(field, op, value); }
    /** @brief Adds a raw WHERE expression */
    Query &where(const QString &expression) { return andWhere(expression); }

    /** @brief Internal helper for AND WHERE equal */
    Query &andWhere(const QString &field, const QVariant &value) { return andWhere(field, "=", value); }
    /** @brief Adds an AND WHERE clause with a custom operator */
    Query &andWhere(const QString &field, const QString &op, const QVariant &value);
    /** @brief Adds a raw AND WHERE expression */
    Query &andWhere(const QString &expression);

    /** @brief Internal helper for OR WHERE equal */
    Query &orWhere(const QString &field, const QVariant &value) { return orWhere(field, "=", value); }
    /** @brief Adds an OR WHERE clause with a custom operator */
    Query &orWhere(const QString &field, const QString &op, const QVariant &value);
    /** @brief Adds a raw OR WHERE expression */
    Query &orWhere(const QString &expression);

    /** @brief Adds a GROUP BY clause */
    Query &groupBy(const QString &field);

    /** @brief Adds an ORDER BY clause */
    Query &orderBy(const QString &field, Qt::SortOrder order = Qt::DescendingOrder);

    /** @brief Configures pagination (LIMIT/OFFSET) */
    Query &page(int page, int countPerPage = 20);
    /** @brief Adds a LIMIT clause */
    Query &limit(int limit);
    /** @brief Adds an OFFSET clause */
    Query &offset(int offset);

    /** @brief Adds a relationship for eager loading */
    Query &with(const QString &relation);
    /** @brief Adds multiple relationships for eager loading */
    Query &with(const QStringList &relations);

    /** @brief Returns the connection used by this query */
    Connection connection() const;
    /** @brief Returns the name of the connection */
    QString connectionName() const;
    /** @brief Configures the connection name for the query */
    Query &connection(const QString &connectionName);

    /** @brief Returns true if the query has WHERE clauses */
    bool hasWhere() const;
    /** @brief Returns the generated WHERE clause string */
    QString whereClause() const;
    /** @brief Returns the generated WHERE clause string for a specific connection */
    QString whereClause(const Connection connection) const;

    /** @brief Returns true if the query has GROUP BY clauses */
    bool hasGroupBy() const;
    /** @brief Returns the generated GROUP BY clause string */
    QString groupByClause() const;
    /** @brief Returns the generated GROUP BY clause string for a specific connection */
    QString groupByClause(const Connection connection) const;

    /** @brief Returns true if the query has ORDER BY clauses */
    bool hasOrderBy() const;
    /** @brief Returns the generated ORDER BY clause string */
    QString orderByClause() const;
    /** @brief Returns the generated ORDER BY clause string for a specific connection */
    QString orderByClause(const Connection connection) const;

    /** @brief Returns the generated LIMIT clause string */
    QString limitClause() const;
    /** @brief Returns the generated OFFSET clause string */
    QString offsetClause() const;

    /** @brief Returns the full SQL statement (SELECT) */
    QString toString() const;
    /** @brief Returns the full SQL statement for a specific connection */
    QString toString(const Connection connection) const;

    /** @brief Returns the list of relations to be eager loaded */
    QStringList relations() const;

private:
    QSharedDataPointer<ModelQueryData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_QUERY_H
