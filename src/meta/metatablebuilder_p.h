#ifndef QELOQUENT_METATABLEBUILDER_P_H
#define QELOQUENT_METATABLEBUILDER_P_H

#include "metaobjectbuilder.h"

#include <QEloquent/namingconvention.h>
#include <QEloquent/connection.h>
#include <QEloquent/private/metatable_p.h>

namespace QEloquent {

class MetaObjectBuilderPrivate : public MetaObjectPrivate
{
public:
    MetaObjectBuilderPrivate(const QString &className)
        : convention(NamingConvention::convention(QStringLiteral("Laravel")))
        , connection(Connection::defaultConnection())
    {
        this->className = className;
        this->tableName = convention->tableName(className.section("::", -1));
        this->connectionName = connection.name();
    }

    MetaObjectBuilderPrivate(const MetaObjectPrivate &other)
        : MetaObjectPrivate(other)
        , convention(NamingConvention::convention(QStringLiteral("Laravel")))
        , connection(Connection::connection(other.connectionName))
    {}

    MetaObjectBuilderPrivate(const MetaObjectBuilderPrivate &) = default;

    MetaObjectPrivate *build() const
    { return new MetaObjectPrivate(*this); }

    NamingConvention *convention;
    Connection connection;
};

}

#endif // QELOQUENT_METATABLEBUILDER_P_H
