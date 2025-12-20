#include "metaobjectregistry.h"

#include <QEloquent/metaobject.h>
#include <QEloquent/connection.h>

#include <QList>

namespace QEloquent {

bool MetaObjectRegistry::contains(const QString &className)
{
    auto it = std::find_if(s_metaObjects.constBegin(), s_metaObjects.constEnd(), [&className](const MetaObject &metaObject) {
        return metaObject.className() == className;
    });

    return (it == s_metaObjects.end() ? false : true);
}

MetaObject MetaObjectRegistry::metaObject(const QString &className)
{
    auto it = std::find_if(s_metaObjects.constBegin(), s_metaObjects.constEnd(), [&className](const MetaObject &metaObject) {
        return metaObject.className() == className;
    });

    return (it == s_metaObjects.constEnd() ? MetaObject() : *it);
}

MetaObject MetaObjectRegistry::tableMetaObject(const QString &tableName)
{
    return tableMetaObject(tableName, Connection::defaultConnection().name());
}

MetaObject MetaObjectRegistry::tableMetaObject(const QString &tableName, const QString &connectionName)
{
    auto it = std::find_if(s_metaObjects.constBegin(), s_metaObjects.constEnd(), [&tableName, &connectionName](const MetaObject &metaObject) {
        return metaObject.tableName() == tableName && metaObject.connectionName() == connectionName;
    });

    return (it == s_metaObjects.constEnd() ? MetaObject() : *it);
}

void MetaObjectRegistry::registerMetaObject(const MetaObject &object)
{
    auto it = std::find_if(s_metaObjects.constBegin(), s_metaObjects.constEnd(), [&object](const MetaObject &metaObject) {
        return metaObject.tableName() == object.tableName() && metaObject.connectionName() == object.connectionName();
    });

    if (it == s_metaObjects.constEnd())
        s_metaObjects.append(object);
    else
        s_metaObjects.replace(std::distance(s_metaObjects.constBegin(), it), object);
}

QList<MetaObject> MetaObjectRegistry::s_metaObjects;

} // namespace QEloquent
