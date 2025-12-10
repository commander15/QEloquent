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
    MetaProperty primaryProperty;
    MetaProperty foreignProperty;
    MetaProperty creationTimestamp;
    MetaProperty updateTimestamp;
    MetaProperty deletionTimestamp;
    QList<MetaProperty> properties;
    QString connectionName;

    const QMetaObject *metaObject = nullptr;

    bool hasProperty(const QString &name) const
    {
        auto it = std::find_if(properties.begin(), properties.end(), [&name](const MetaProperty &property) {
            return property.propertyName() == name;
        });

        return (it == properties.end() ? false : true);
    }

    MetaProperty property(const QString &name) const
    {
        auto it = std::find_if(properties.begin(), properties.end(), [&name](const MetaProperty &property) {
            return property.propertyName() == name;
        });

        return (it == properties.end() ? MetaProperty() : *it);
    }
};

}

#endif // QELOQUENT_METAOBJECT_P_H
