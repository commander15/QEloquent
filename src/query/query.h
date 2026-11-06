#ifndef QELOQUENT_QUERY_H
#define QELOQUENT_QUERY_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

namespace QEloquent {

class Connection;
class Driver;

class ModelQueryData;
class QELOQUENT_EXPORT Query
{
public:
    Query();
    Query(const Query &);
    Query(Query &&);
    Query &operator=(const Query &);
    Query &operator=(Query &&);
    ~Query();

    QString tableName() const;
    Query &table(const QString &tableName);

    bool hasRawSql() const;
    QString rawSql() const;
    Query &raw(const QString &statement);

    Query &where(const QString &field, const QVariant &value) { return andWhere(field, "=", value); }
    Query &where(const QString &field, const QString &op, const QVariant &value) { return andWhere(field, op, value); }
    Query &where(const QString &expression) { return andWhere(expression); }

    Query &andWhere(const QString &field, const QVariant &value) { return andWhere(field, "=", value); }
    Query &andWhere(const QString &field, const QString &op, const QVariant &value);
    Query &andWhere(const QString &expression);

    Query &orWhere(const QString &field, const QVariant &value) { return orWhere(field, "=", value); }
    Query &orWhere(const QString &field, const QString &op, const QVariant &value);
    Query &orWhere(const QString &expression);

    Query &join(const QString &table, const QString &first, const QString &op, const QString &second, const QString &type = "INNER");

    Query &groupBy(const QString &field);

    Query &orderBy(const QString &field, Qt::SortOrder order = Qt::DescendingOrder);

    Query &page(int page, int countPerPage = 20);
    Query &limit(int limit);
    Query &offset(int offset);

    Query &with(const QString &relation);
    Query &with(const QStringList &relations);

    Connection connection() const;
    QString connectionName() const;
    Query &connection(const QString &connectionName);

    bool hasJoins() const;
    QString joinClauses() const;
    QString joinClauses(const Connection &connection) const;
    QString joinClauses(const Driver *driver) const;

    bool hasWhere() const;
    QString whereClause() const;
    QString whereClause(const Connection &connection) const;
    QString whereClause(const Driver *driver) const;

    bool hasGroupBy() const;
    QString groupByClause() const;
    QString groupByClause(const Connection &connection) const;
    QString groupByClause(const Driver *driver) const;

    bool hasOrderBy() const;
    QString orderByClause() const;
    QString orderByClause(const Connection &connection) const;
    QString orderByClause(const Driver *driver) const;

    QString limitClause() const;
    QString offsetClause() const;

    QString toString() const;
    QString toString(const Connection &connection) const;
    QString toString(const Driver *driver) const;

    QStringList relations() const;

private:
    QSharedDataPointer<ModelQueryData> data;
};

template<typename Model>
class QueryHelper
{
public:
    QueryHelper() : m_query(makeQuery()) {}
    QueryHelper(const Query &query) : m_query(query) {}
    QueryHelper(Query &&query) : m_query(std::move(query)) {}

    // --- Fluent Query Builder Forwarding ---

    QueryHelper &where(const QString &field, const QVariant &value) { m_query.where(field, value); return *this; }
    QueryHelper &where(const QString &field, const QString &op, const QVariant &value) { m_query.where(field, op, value); return *this; }
    QueryHelper &where(const QString &expression) { m_query.where(expression); return *this; }

    QueryHelper &andWhere(const QString &field, const QVariant &value) { m_query.andWhere(field, value); return *this; }
    QueryHelper &andWhere(const QString &field, const QString &op, const QVariant &value) { m_query.andWhere(field, op, value); return *this; }
    QueryHelper &andWhere(const QString &expression) { m_query.andWhere(expression); return *this; }

    QueryHelper &orWhere(const QString &field, const QVariant &value) { m_query.orWhere(field, value); return *this; }
    QueryHelper &orWhere(const QString &field, const QString &op, const QVariant &value) { m_query.orWhere(field, op, value); return *this; }
    QueryHelper &orWhere(const QString &expression) { m_query.orWhere(expression); return *this; }

    QueryHelper &join(const QString &table, const QString &first, const QString &op, const QString &second, const QString &type = "INNER") {
        m_query.join(table, first, op, second, type);
        return *this;
    }

    QueryHelper &groupBy(const QString &field) { m_query.groupBy(field); return *this; }
    QueryHelper &orderBy(const QString &field, Qt::SortOrder order = Qt::DescendingOrder) { m_query.orderBy(field, order); return *this; }

    QueryHelper &page(int page, int countPerPage = 20) { m_query.page(page, countPerPage); return *this; }
    QueryHelper &limit(int limit) { m_query.limit(limit); return *this; }
    QueryHelper &offset(int offset) { m_query.offset(offset); return *this; }

    QueryHelper &with(const QString &relation) { m_query.with(relation); return *this; }
    QueryHelper &with(const QStringList &relations) { m_query.with(relations); return *this; }

    QueryHelper &connection(const QString &connectionName) { m_query.connection(connectionName); return *this; }

    // Allow implicit or explicit conversion back to a plain Query if needed elsewhere
    operator Query() const { return m_query; }
    const Query &query() const { return m_query; }
    Query &query() { return m_query; }

    // --- Execution & Conversion Operators ---
    QList<Model> get() {
        if (!m_executed) {
            m_models = Model::find(m_query);
            m_executed = true;
        }
        return m_models;
    }

    operator Model() {
        if (!m_executed)
            get();
        return (m_models.isEmpty() ? Model() : m_models.first());
    }

    operator QList<Model>() {
        if (!m_executed)
            get();
        return m_models;
    }

    static QueryHelper<Model> helper(Query query) {
        return QueryHelper<Model>(std::move(query));
    }

private:
    static Query makeQuery() { Query q; q.table(""); return q; }

    Query m_query;
    QList<Model> m_models;
    bool m_executed = false;
};

} // namespace QEloquent

#endif // QELOQUENT_QUERY_H
