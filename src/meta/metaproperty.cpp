#include "metaproperty.h"
#include "metaproperty_p.h"

#include <QEloquent/model.h>
#include "../orm/model_p.h"

#include <QMetaType>
#include <QVariant>

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
    if (data->propertyType == StandardProperty) {
        return (data->metaProperty.isValid() ? data->metaProperty.readOnGadget(model) : QVariant());
    }

    if (data->propertyType == DynamicProperty) {
        return model->data->dynamicProperties.value(data->propertyName);
    }

    if (data->propertyType == AppendedProperty) {
        if (data->getter.isValid()) {
            void *value = data->metaType.create();
            data->getter.invokeOnGadget(const_cast<Model *>(model), QGenericReturnArgument(data->metaType.name(), value));
            const QVariant var = QVariant::fromMetaType(data->metaType, value);
            data->metaType.destroy(value);
            return var;
        } else {
            return QVariant();
        }
    }

    if (data->propertyType == RelationProperty) {
        if (data->getter.isValid()) {
            void *value = data->metaType.create();
            data->getter.invokeOnGadget(const_cast<Model *>(model), QGenericReturnArgument(data->metaType.name(), value));
            const QVariant var = QVariant::fromMetaType(data->metaType, value);
            data->metaType.destroy(value);
            return var;
        } else {
            return QVariant();
        }
    }

    return QVariant();
}

bool MetaProperty::write(Model *model, const QVariant &value) const
{
    if (data->propertyType == StandardProperty) {
        return data->metaProperty.writeOnGadget(model, value);
    }

    if (data->propertyType == DynamicProperty) {
        model->data->dynamicProperties.insert(data->propertyName, value);
        return true;
    }

    return false;
}

bool MetaProperty::isValid() const
{
    return !data->propertyName.isEmpty() && data->metaType.isValid() && data->isReadable();
}

} // namespace QEloquent
