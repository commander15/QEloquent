#ifndef QELOQUENT_MODELQUERY_H
#define QELOQUENT_MODELQUERY_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

namespace QEloquent {

class Connection;

class ModelQueryData;
class ModelQuery
{
public:
    ModelQuery();
    ModelQuery(const ModelQuery &);
    ModelQuery(ModelQuery &&);
    ModelQuery &operator=(const ModelQuery &);
    ModelQuery &operator=(ModelQuery &&);
    ~ModelQuery();

    ModelQuery &where(const QString &field, const QVariant &value) { return andWhere(field, "=", value); }
    ModelQuery &where(const QString &field, const QString &op, const QVariant &value) { return andWhere(field, op, value); }
    ModelQuery &where(const QString &expression) { return andWhere(expression); }

    ModelQuery &andWhere(const QString &field, const QVariant &value) { return andWhere(field, "=", value); }
    ModelQuery &andWhere(const QString &field, const QString &op, const QVariant &value);
    ModelQuery &andWhere(const QString &expression);

    ModelQuery &orWhere(const QString &field, const QVariant &value) { return orWhere(field, "=", value); }
    ModelQuery &orWhere(const QString &field, const QString &op, const QVariant &value);
    ModelQuery &orWhere(const QString &expression);

    ModelQuery &groupBy(const QString &field);

    ModelQuery &orderBy(const QString &field, Qt::SortOrder order = Qt::DescendingOrder);

    ModelQuery &page(int page, int countPerPage = 20);
    ModelQuery &limit(int limit);
    ModelQuery &offset(int offset);

    ModelQuery &with(const QString &relation);
    ModelQuery &with(const QStringList &relations);

    bool hasWhere() const;
    QString whereClause(const Connection &connection) const;

    bool hasGroupBy() const;
    QString groupByClause(const Connection &connection) const;

    QString orderByClause(const Connection &connection) const;
    QString limitClause() const;
    QString offsetClause() const;
    QString toString(const Connection &connection) const;
    QString toString() const;

    QStringList relations() const;

private:
    QSharedDataPointer<ModelQueryData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_MODELQUERY_H
