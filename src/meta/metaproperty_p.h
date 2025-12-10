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
    { return !propertyName.isEmpty(); }

    bool isWritable() const
    { return !propertyName.isEmpty() && propertyType != MetaProperty::AppendedProperty; }
};

}

#endif // METAPROPERTY_P_H
