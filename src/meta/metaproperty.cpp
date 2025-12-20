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
    if (data->metaProperty.isValid() && data->metaProperty.isReadable())
        return data->metaProperty.readOnGadget(model);

    if (data->getter.isValid()) {
        QVariant value;
        data->getter.invokeOnGadget(const_cast<Model *>(model), Q_RETURN_ARG(QVariant, value));
        return value;
    }

    return model->data->dynamicProperties.value(data->propertyName);
}

bool MetaProperty::write(Model *model, const QVariant &value) const
{
    if (data->metaProperty.isValid())
        return (data->metaProperty.isWritable() ? data->metaProperty.writeOnGadget(model, value) : false);

    if (data->getter.isValid() && data->propertyType == RelationProperty) {
        QVariant relation;
        data->getter.invokeOnGadget(model, Q_RETURN_ARG(QVariant, relation));
        return false;
    }

    if (data->setter.isValid())
        return data->setter.invokeOnGadget(model, value);

    if (data->propertyType != AppendedProperty) {
        QVariantMap *properties = &model->data->dynamicProperties;
        if ((!value.isValid() || value.isNull()) && properties->contains(data->propertyName))
            properties->remove(data->propertyName);
        else
            properties->insert(data->propertyName, value);
        return true;
    }

    return false;
}

bool MetaProperty::isValid() const
{
    return !data->propertyName.isEmpty() && data->metaType.isValid() && data->isReadable();
}

} // namespace QEloquent
