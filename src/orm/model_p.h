#ifndef QLOQUENT_ORM_MODEL_P_H
#define QLOQUENT_ORM_MODEL_P_H

#include "model.h"

#include <QEloquent/query.h>
#include <QEloquent/error.h>
#include <QEloquent/metaobject.h>
#include <QEloquent/queryrunner.h>
#include <QEloquent/connection.h>

#include <QVariant>
#include <QMetaProperty>
#include <QSqlQuery>

namespace QEloquent {

class ModelData : public QSharedData
{
public:
    QVariantMap dynamicProperties;
    QMap<QString, QExplicitlySharedDataPointer<RelationData>> relationData;
    MetaObject metaObject;

    Query lastQuery;
    Error lastError;

    Result<::QSqlQuery, ::QSqlError> exec(const QString &statement, Query query, Model *model)
    {
        query
            .raw(statement)
            .table(metaObject.tableName())
            .connection(metaObject.connectionName());

        auto result = QueryRunner::exec(statement, query.connection());
        lastQuery = query.raw(statement);
        if (!result)
            lastError = Error::fromSqlError(result.error());
        return result;
    }
};

} // namespace QELoquent

#endif // QLOQUENT_ORM_MODEL_P_H
