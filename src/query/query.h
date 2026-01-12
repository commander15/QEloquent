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

} // namespace QEloquent

#endif // QELOQUENT_QUERY_H
