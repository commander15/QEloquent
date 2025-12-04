#ifndef QELOQUEN_QUERYBUILDER_H
#define QELOQUEN_QUERYBUILDER_H

#include <QEloquent/global.h>

class QJsonObject;

namespace QEloquent {

class Connection;
class ModelQuery;
class ModelInfo;

class QELOQUENT_EXPORT QueryBuilder
{
public:
    static QString selectStatement(const ModelQuery &query, const ModelInfo &info);
    static QString insertStatement(const QVariantMap &data, const ModelInfo &info);
    static QString updateStatement(const ModelQuery &query, const QVariantMap &data, const ModelInfo &info);
    static QString deleteStatement(const ModelQuery &query, const ModelInfo &info);
    static QString whereClause(const ModelQuery &query, const Connection &connection);

    static QString singularise(const QString &tableName);

    static QString escapeFieldName(const QString &name, const Connection &connection);
    static QString escapeTableName(const QString &name, const Connection &connection);

    static QString formatValue(const QVariant &value, const Connection &connection);
    static QString formatValue(const QVariant &value, const QMetaType &type, const Connection &connection);

    static QStringList statementsFromScript(const QString &script);
};

} // namespace QEloquent

#endif // QELOQUEN_QUERYBUILDER_H
