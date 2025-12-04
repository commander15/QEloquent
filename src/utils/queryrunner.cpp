#include "queryrunner.h"

#include <QEloquent/connection.h>

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

expected<QSqlQuery, QSqlError> QueryRunner::exec(const QString &statement, const Connection &connection)
{
    QSqlQuery query(connection.database());
    query.setForwardOnly(true);
    if (query.exec(statement))
        return query;
    else
        return std::unexpected(query.lastError());
}

} // namespace QEloquent
