#ifndef METAPROPERTY_P_H
#define METAPROPERTY_P_H

#include "metaproperty.h"

#include <QMetaType>
#include <QMetaProperty>
#include <QMetaMethod>

namespace QEloquent {

class MetaPropertyData : public QSharedData
{
public:
    QString propertyName;
    QString fieldName;
    QMetaType metaType;
    MetaProperty::PropertyAttributes attributes;
    MetaProperty::PropertyType propertyType;

    QMetaProperty metaProperty;
    QMetaMethod getter;
    QMetaMethod setter;

    bool isReadable() const
    { return (metaProperty.isValid() && metaProperty.isReadable()) || getter.isValid(); }

    bool isWritable() const
    { return (metaProperty.isValid() && metaProperty.isWritable()) || setter.isValid() || propertyType == MetaProperty::RelationProperty; }
};

}

#endif // METAPROPERTY_P_H
