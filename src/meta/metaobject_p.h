#ifndef QELOQUENT_METAOBJECT_P_H
#define QELOQUENT_METAOBJECT_P_H

#include "metaobject.h"

#include <QEloquent/metaproperty.h>

#include <QMap>
#include <QMetaType>

namespace QEloquent {

class MetaObjectPrivate : public QSharedData
{
public:
    virtual ~MetaObjectPrivate() = default;

    QString tableName;

    int primaryPropertyIndex = -1;
    int creationTimestampIndex = -1;
    int updateTimestampIndex = -1;
    int deletionTimestampIndex = -1;
    QList<MetaProperty> properties;
    MetaProperty foreignProperty;
    QString connectionName;

    const QMetaObject *metaObject = nullptr;
};

}

#endif // QELOQUENT_METAOBJECT_P_H
