#ifndef QELOQUENT_METAOBJECTREGISTRY_H
#define QELOQUENT_METAOBJECTREGISTRY_H

#include <QEloquent/global.h>

namespace QEloquent {

class MetaObject;

class QELOQUENT_EXPORT MetaObjectRegistry
{
public:
    static MetaObject tableMetaObject(const QString &tableName);
    static MetaObject tableMetaObject(const QString &tableName, const QString &connectionName);
    //static MetaObject classMetaObject(const QString &className);
    static void registerMetaObject(const MetaObject &object);

private:
    static QList<MetaObject> s_metaObjects;
};

} // namespace QEloquent

#endif // QELOQUENT_METAOBJECTREGISTRY_H
