#ifndef QELOQUENT_QUERYRUNNER_H
#define QELOQUENT_QUERYRUNNER_H

#include <QEloquent/global.h>
#include <QEloquent/result.h>

class QSqlQuery;
class QSqlError;

namespace QEloquent {

class Connection;

class QELOQUENT_EXPORT QueryRunner
{
public:
    static Result<QSqlQuery, QSqlError> exec(const QString &statement);
    static Result<QSqlQuery, QSqlError> exec(const QString &statement, const QString &connectionName);
    static Result<QSqlQuery, QSqlError> exec(const QString &statement, const Connection &connection);
};

} // namespace QEloquent

#endif // QELOQUENT_QUERYRUNNER_H
