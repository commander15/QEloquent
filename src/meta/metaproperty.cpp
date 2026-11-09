#include "metaproperty.h"
#include "metaproperty_p.h"

#include <QEloquent/model_a.h>
#include <QEloquent/data.h>
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

bool MetaProperty::isNull(const Model *model) const
{
    if (model == nullptr)
        return true;

    return model->isPropertyNull(data->propertyName);
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
    if (isNull(model))
        return QVariant();

    QVariant value = readRaw(model);

    const AbstractData *d = AbstractData::fromVariant(value);
    if (d != nullptr)
        value = d->value();

    return value;
}

bool MetaProperty::write(Model *model, const QVariant &value) const
{
    if (model == nullptr)
        return false;

    QVariant val = value;

    // Force UTC on datetime
    if (!val.isNull() && data->metaType == QMetaType::fromType<QDateTime>()) {
        QDateTime timestamp = val.toDateTime();
        if (timestamp.timeZone() != QTimeZone::utc()) {
            timestamp.setTimeZone(QTimeZone::utc());
            val = timestamp;
        }
    }

    QVariant raw = readRaw(model);
    AbstractData *d = AbstractData::fromVariant(raw);
    if (d != nullptr) {
        if (!d->setValue(val))
            return false;
        val = raw;
    }

    return writeRaw(model, val);
}

bool MetaProperty::generate(Model *model, bool overwrite) const
{
    QVariant data = readRaw(model);
    AbstractData *d = AbstractData::fromVariant(data);

    if (d == nullptr)
        return false;

    if (!overwrite && !d->isNull())
        return true;

    QString connection = model->connection().name();
    if (!d->generate(connection))
        return false;

    return writeRaw(model, data);
}

bool MetaProperty::isValid() const
{
    return !data->propertyName.isEmpty() && data->metaType.isValid() && data->isReadable();
}

QVariant MetaProperty::readRaw(const Model *model) const
{
    QVariant value;

    if (model == nullptr)
        return value;

    if (data->propertyType == StandardProperty && data->metaProperty.isValid()) {
        value = data->metaProperty.readOnGadget(model);
    }

    if (data->propertyType == DynamicProperty && model->data->dynamicProperties.contains(data->propertyName)) {
        value = model->data->dynamicProperties.value(data->propertyName);
    }

    if (data->propertyType == AppendedProperty && data->getter.isValid()) {
        void *val = data->metaType.create();
        data->getter.invokeOnGadget(const_cast<Model *>(model), QGenericReturnArgument(data->metaType.name(), val));
        value = QVariant(data->metaType, val);
        data->metaType.destroy(val);
    }

    if (data->propertyType == RelationProperty && data->getter.isValid()) {
        void *val = data->metaType.create();
        data->getter.invokeOnGadget(const_cast<Model *>(model), QGenericReturnArgument(data->metaType.name(), val));
        value = QVariant(data->metaType, val);
        data->metaType.destroy(val);
    }

    return value;
}

bool MetaProperty::writeRaw(Model *model, const QVariant &value) const
{
    if (model == nullptr)
        return false;

    if (data->propertyType == StandardProperty && data->metaProperty.isValid()) {
        return data->metaProperty.writeOnGadget(model, value);
    }

    if (data->propertyType == DynamicProperty) {
        DataMap &dynamic = model->data->dynamicProperties;
        dynamic.insert(data->propertyName, value);
        return true;
    }

    return false;
}

} // namespace QEloquent
