#ifndef QELOQUENT_QUERYRUNNER_H
#define QELOQUENT_QUERYRUNNER_H

#include <QEloquent/global.h>
#include <QEloquent/result.h>

class QSqlQuery;
class QSqlError;

namespace QEloquent {

class Query;
class Connection;
class DataMap;

class QELOQUENT_EXPORT QueryRunner
{
public:
    static Result<QSqlQuery, QSqlError> select(const Query &query);
    static Result<QSqlQuery, QSqlError> select(const QList<QPair<QString, QString>> &fields, const Query &query);
    static Result<QSqlQuery, QSqlError> select(const QStringList fields, const Query &query);
    static Result<QSqlQuery, QSqlError> select(const QString &fields, const Query &query);

    static Result<QSqlQuery, QSqlError> count(const Query &query);

    static Result<QSqlQuery, QSqlError> insert(const DataMap &data, const Query &query);

    static Result<QSqlQuery, QSqlError> update(const DataMap &data, const Query &query);

    static Result<QSqlQuery, QSqlError> deleteData(const Query &query);

#ifdef QELOQUENT_MIGRATIONS_SUPPORT
    static Result<QSqlQuery, QSqlError> createTable(const QString &tableName, const class TableBlueprint &blueprint);
    static Result<QSqlQuery, QSqlError> createTable(const QString &tableName, const class TableBlueprint &blueprint, const QString &connectionName);
    static Result<QSqlQuery, QSqlError> createTable(const QString &tableName, const class TableBlueprint &blueprint, const Connection &connection);
#endif

    static Result<QSqlQuery, QSqlError> exec(const QString &statement);
    static Result<QSqlQuery, QSqlError> exec(const QString &statement, const QString &connectionName);
    static Result<QSqlQuery, QSqlError> exec(const QString &statement, const Connection &connection);
};

} // namespace QEloquent

#endif // QELOQUENT_QUERYRUNNER_H
