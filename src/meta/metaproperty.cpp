#include "metaproperty.h"
#include "metaproperty_p.h"

#include <QEloquent/model.h>
#include <QEloquent/private/model_p.h>

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
    if (!data->isReadable())
        return QVariant();

    if (data->metaProperty.isValid())
        return data->metaProperty.readOnGadget(model);

    if (data->getter.isValid())
        return data->getter.invokeOnGadget(const_cast<Model *>(model));

    return model->data->dynamicProperties.value(data->propertyName);
}

bool MetaProperty::write(Model *model, const QVariant &value) const
{
    if (!data->isWritable())
        return false;

    if (data->metaProperty.isValid())
        return data->metaProperty.writeOnGadget(model, value);

    if (data->setter.isValid())
        return data->setter.invokeOnGadget(model, value);

    QVariantMap *properties = &model->data->dynamicProperties;
    if ((!value.isValid() || value.isNull()) && properties->contains(data->propertyName))
        properties->remove(data->propertyName);
    else
        properties->insert(data->propertyName, value);
    return true;
}

bool MetaProperty::isValid() const
{
    return !data->propertyName.isEmpty() && data->metaType.isValid();
}

} // namespace QEloquent
