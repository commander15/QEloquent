#ifndef QELOQUEN_QUERYBUILDER_H
#define QELOQUEN_QUERYBUILDER_H

#include <QEloquent/global.h>

class QIODevice;

namespace QEloquent {

class Query;
class Connection;

class QELOQUENT_EXPORT QueryBuilder
{
public:
    static QString selectStatement(const Query &query);
    static QString selectStatement(const QList<QPair<QString, QString>> &fields, const Query &query);
    static QString selectStatement(const QStringList fields, const Query &query);
    static QString selectStatement(const QString fields, const Query &query);

    static QString insertStatement(const QVariantMap &data, const Query &query);

    static QString updateStatement(const QVariantMap &data, const Query &query);

    static QString deleteStatement(const Query &query);

    static QString escapeFieldName(const QString &name, const Connection &connection);
    static QString escapeTableName(const QString &name, const Connection &connection);

    static QString formatValue(const QVariant &value, const Connection &connection);
    static QString formatValue(const QVariant &value, const QMetaType &type, const Connection &connection);

    static QStringList statementsFromScriptFile(const QString &fileName);
    static QStringList statementsFromScriptDevice(QIODevice *device);
    static QStringList statementsFromScriptContent(const QByteArray &content);

    static QString singularise(const QString &word);
};

} // namespace QEloquent

#endif // QELOQUEN_QUERYBUILDER_H
