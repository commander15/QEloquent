#include "metaproperty.h"
#include "metaproperty_p.h"

#include <QEloquent/model.h>
#include "../orm/model_p.h"

#include <QMetaType>
#include <QVariant>
#include <QDateTime>
#include <QTimeZone>

namespace QEloquent {

MetaProperty::MetaProperty()
    : data(new MetaPropertyData)
{}

MetaProperty::MetaProperty(MetaPropertyData *data)
    : data(data)
{}

MetaProperty::MetaProperty(const MetaProperty &rhs)
    : data{rhs.data}
{}

MetaProperty::MetaProperty(MetaProperty &&rhs)
    : data{std::move(rhs.data)}
{}

MetaProperty &MetaProperty::operator=(const MetaProperty &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

MetaProperty &MetaProperty::operator=(MetaProperty &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

MetaProperty::~MetaProperty() {}

QString MetaProperty::propertyName() const
{
    return data->propertyName;
}

QString MetaProperty::fieldName() const
{
    return data->fieldName;
}

QMetaType MetaProperty::metaType() const
{
    return data->metaType;
}

bool MetaProperty::hasAttribute(PropertyAttributeFlag flag) const
{
    return data->attributes.testFlag(flag);
}

MetaProperty::PropertyAttributes MetaProperty::attributes() const
{
    return data->attributes;
}

MetaProperty::PropertyType MetaProperty::propertyType() const
{
    return data->propertyType;
}

QVariant MetaProperty::read(const Model *model) const
{
    QVariant value;

    if (data->propertyType == StandardProperty && data->metaProperty.isValid()) {
        value = data->metaProperty.readOnGadget(model);
    }

    if (data->propertyType == DynamicProperty && model->data->dynamicProperties.contains(data->propertyName)) {
        value = model->data->dynamicProperties.value(data->propertyName);
    }

    if (data->propertyType == AppendedProperty && data->getter.isValid()) {
        void *val = data->metaType.create();
        data->getter.invokeOnGadget(const_cast<Model *>(model), QGenericReturnArgument(data->metaType.name(), val));
        value = QVariant::fromMetaType(data->metaType, val);
        data->metaType.destroy(val);
    }

    if (data->propertyType == RelationProperty && data->getter.isValid()) {
        void *val = data->metaType.create();
        data->getter.invokeOnGadget(const_cast<Model *>(model), QGenericReturnArgument(data->metaType.name(), val));
        value = QVariant::fromMetaType(data->metaType, val);
        data->metaType.destroy(val);
    }

    return value;
}

bool MetaProperty::write(Model *model, const QVariant &value) const
{
    QVariant val = value;

    // Force UTC on datetime
    if (data->metaType == QMetaType::fromType<QDateTime>()) {
        QDateTime timestamp = val.toDateTime();
        if (timestamp.timeZone().timeSpec() != Qt::UTC) {
            timestamp.setTimeZone(QTimeZone::UTC);
            val = timestamp;
        }
    }

    if (data->propertyType == StandardProperty) {
        return data->metaProperty.writeOnGadget(model, val);
    }

    if (data->propertyType == DynamicProperty) {
        DataMap &dynamic = model->data->dynamicProperties;
        dynamic.insert(data->propertyName, val);
        return true;
    }

    return false;
}

bool MetaProperty::isValid() const
{
    return !data->propertyName.isEmpty() && data->metaType.isValid() && data->isReadable();
}

} // namespace QEloquent
