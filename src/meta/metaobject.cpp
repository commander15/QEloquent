#include "metaobject.h"
#include "metaobject_p.h"

#include <QEloquent/datamap.h>
#include <QEloquent/metaproperty.h>
#include <QEloquent/metaobjectgenerator.h>
#include <QEloquent/namingconvention.h>
#include <QEloquent/connection.h>
#include <QEloquent/model_a.h>
#include <QEloquent/private/model_p.h>
#include <QEloquent/private/metaproperty_p.h>

namespace QEloquent {

MetaObject::MetaObject()
    : d(new MetaObjectPrivate())
{}

MetaObject::MetaObject(MetaObjectPrivate *data)
    : d(data)
{}

MetaObject::MetaObject(const MetaObject &rhs)
    : d{rhs.d}
{}

MetaObject::MetaObject(MetaObject &&rhs)
    : d{std::move(rhs.d)}
{}

MetaObject &MetaObject::operator=(const MetaObject &rhs)
{
    if (this != &rhs)
        d = rhs.d;
    return *this;
}

MetaObject &MetaObject::operator=(MetaObject &&rhs)
{
    if (this != &rhs)
        d = std::move(rhs.d);
    return *this;
}

MetaObject::~MetaObject()
{}

QString MetaObject::className() const
{
    return (d->metaObject == nullptr ? QString() : QString(d->metaObject->className()));
}

QString MetaObject::tableName() const
{
    return d->tableName;
}

MetaProperty MetaObject::primaryProperty() const
{
    if (d->primaryPropertyIndex > -1 && d->primaryPropertyIndex < d->properties.size())
        return d->properties.at(d->primaryPropertyIndex);
    else
        return MetaProperty();
}

MetaProperty MetaObject::foreignProperty() const
{
    return d->foreignProperty;
}

bool MetaObject::hasLabelProperty() const
{
    return d->labelPropertyIndex >= 0;
}

MetaProperty MetaObject::labelProperty() const
{
    if (d->labelPropertyIndex >= 0 && d->labelPropertyIndex < d->properties.size())
        return d->properties.at(d->labelPropertyIndex);
    else
        return MetaProperty();
}

bool MetaObject::hasCreationTimestamp() const
{
    return d->creationTimestampIndex >= 0;
}

MetaProperty MetaObject::creationTimestamp() const
{
    if (d->creationTimestampIndex >= 0 &&
        static_cast<std::size_t>(d->creationTimestampIndex) < d->properties.size())
    {
        return d->properties.at(d->creationTimestampIndex);
    }
    return MetaProperty(); // invalid/empty property
}

bool MetaObject::hasUpdateTimestamp() const
{
    return d->updateTimestampIndex >= 0;
}

MetaProperty MetaObject::updateTimestamp() const
{
    if (d->updateTimestampIndex >= 0 &&
        static_cast<std::size_t>(d->updateTimestampIndex) < d->properties.size())
    {
        return d->properties.at(d->updateTimestampIndex);
    }
    return MetaProperty();
}

bool MetaObject::hasDeletionTimestamp() const
{
    return d->deletionTimestampIndex >= 0;
}

MetaProperty MetaObject::deletionTimestamp() const
{
    if (d->deletionTimestampIndex >= 0 &&
        static_cast<std::size_t>(d->deletionTimestampIndex) < d->properties.size())
    {
        return d->properties.at(d->deletionTimestampIndex);
    }
    return MetaProperty();
}

MetaProperty MetaObject::property(const QString &name, PropertyNameResolution resolution) const
{
    auto it = std::find_if(d->properties.constBegin(), d->properties.constEnd(), [&name, &resolution](const MetaProperty &property) {
        switch (resolution) {
        case ResolveByPropertyName:
            return property.propertyName() == name;

        case ResolveByFieldName:
            return property.fieldName() == name;

        default:
            return false;
        }
    });

    return (it == d->properties.constEnd() ? MetaProperty() : *it);
}

QList<MetaProperty> MetaObject::properties(
    MetaProperty::PropertyAttributes attributes,
    PropertyFilters filters) const
{
    return properties([&filters, &attributes](const MetaProperty &prop) {
        // Filter by type
        bool typeMatch = false;
        if (filters == AllProperties) {
            typeMatch = true;
        } else {
            switch (prop.propertyType()) {
            case MetaProperty::StandardProperty:
                typeMatch = filters.testFlag(StandardProperties);
                break;

            case MetaProperty::DynamicProperty:
                typeMatch = filters.testFlag(DynamicProperties);
                break;

            case MetaProperty::AppendedProperty:
                typeMatch = filters.testFlag(AppendedProperties);
                break;

            case MetaProperty::RelationProperty:
                typeMatch = filters.testFlag(RelationProperties);
                break;
            }
        }
        if (!typeMatch) return false;

        // Filter by attributes
        bool filterMatch;
        if (attributes == MetaProperty::NoAttibutes)
            filterMatch = true;
        else
            filterMatch = (prop.attributes().testAnyFlags(attributes));
        if (!filterMatch)
            return false;

        return true;
    });
    QList<MetaProperty> result = d->properties;

    result.removeIf([&filters, &attributes](const MetaProperty &p) {
        // Filter by type
        bool typeMatch = false;
        if (filters == AllProperties) {
            typeMatch = true;
        } else {
            switch (p.propertyType()) {
            case MetaProperty::StandardProperty:
                typeMatch = filters.testFlag(StandardProperties);
                break;

            case MetaProperty::DynamicProperty:
                typeMatch = filters.testFlag(DynamicProperties);
                break;

            case MetaProperty::AppendedProperty:
                typeMatch = filters.testFlag(AppendedProperties);
                break;

            case MetaProperty::RelationProperty:
                typeMatch = filters.testFlag(RelationProperties);
                break;
            }
        }
        if (!typeMatch) return true;

        // Filter by attributes
        bool filterMatch;
        if (attributes == MetaProperty::NoAttibutes)
            filterMatch = true;
        else
            filterMatch = (p.attributes().testAnyFlags(attributes));
        if (!filterMatch)
            return true;

        return false;
    });

    return result;
}

QList<MetaProperty> MetaObject::properties(PropertyFilters filters) const
{
    if (filters == AllProperties)
        return d->properties;

    return properties([&filters](const MetaProperty &prop) {
        switch (prop.propertyType()) {
        case MetaProperty::StandardProperty:
            return filters.testFlag(StandardProperties);

        case MetaProperty::DynamicProperty:
            return filters.testFlag(DynamicProperties);

        case MetaProperty::AppendedProperty:
            return filters.testFlag(AppendedProperties);

        case MetaProperty::RelationProperty:
            return filters.testFlag(RelationProperties);
        }

        return false;
    });
}

QList<MetaProperty> MetaObject::properties(const std::function<bool (const MetaProperty &)> &pred) const
{
    QList<MetaProperty> result;
    std::copy_if(d->properties.begin(), d->properties.end(), std::back_inserter(result), pred);
    return result;
}

DataMap MetaObject::read(const Model *model,
                         MetaProperty::PropertyAttributes attributes,
                         PropertyFilters filters,
                         PropertyNameResolution resolution) const
{
    const QList<MetaProperty> properties = this->properties(attributes, filters);
    return read(model, properties, resolution);
}

DataMap MetaObject::read(const Model *model, const QList<MetaProperty> &properties, PropertyNameResolution resolution) const
{
    DataMap data;

    for (const MetaProperty &property : properties) {
        QString name;
        switch (resolution) {
        case ResolveByPropertyName:
            name = property.propertyName();
            break;

        case ResolveByFieldName:
            name = property.fieldName();
            break;
        }

        if (property.propertyType() == MetaProperty::RelationProperty) {
            property.read(model); // We trigger lazy load
            if (!model->data->relationData.contains(property.propertyName())) continue;

            auto relation = model->data->relationData.value(property.propertyName());
            const QList<DataMap> maps = relation->serialize(false);
            if (relation->isListSerializable())
                data.insert(name, maps);
            else if (maps.isEmpty())
                data.insert(name, QVariant());
            else
                data.insert(name, maps.first());
        } else {
            data.insert(name, property.read(model));
        }
    }

    return data;
}

DataMap MetaObject::read(const Model *model, PropertyNameResolution resolution) const
{
    return read(model, d->properties, resolution);
}

int MetaObject::write(Model *model, const DataMap &data, PropertyNameResolution resolution) const
{
    int written = true;

    const QStringList properties = data.keys();
    for (const QString &propertyName : properties) {
        const MetaProperty property = this->property(propertyName, resolution);
        if (!property.isValid())
            continue;

        if (property.write(model, data.value(propertyName)))
            ++written;
    }

    return written;
}

DataMap MetaObject::readFillableFields(const Model *model) const
{
    return read(model,
                MetaProperty::FillableProperty,
                StandardProperties | DynamicProperties,
                ResolveByFieldName);
}

bool MetaObject::writeFillableFields(Model *model, const DataMap &data)
{
    bool allWritten = true;

    const QStringList fields = data.keys();
    for (const MetaProperty &property : std::as_const(d->properties))
        if (property.hasAttribute(MetaProperty::FillableProperty) && fields.contains(property.fieldName()))
            if (!property.write(model, data.value(property.fieldName())))
                allWritten = false;

    return allWritten;
}

QStringList MetaObject::hiddenFieldNames() const
{
    QStringList fields;
    for (const MetaProperty &property : std::as_const(d->properties))
        if (property.hasAttribute(MetaProperty::HiddenProperty))
            fields.append(property.fieldName());
    return fields;
}

QStringList MetaObject::fillableFieldNames() const
{
    QStringList fields;
    for (const MetaProperty &property : std::as_const(d->properties))
        if (property.hasAttribute(MetaProperty::FillableProperty))
            fields.append(property.fieldName());
    return fields;
}

QStringList MetaObject::appendFieldNames() const
{
    QStringList fields;
    for (const MetaProperty &property : std::as_const(d->properties))
        if (property.propertyType() == MetaProperty::AppendedProperty)
            fields.append(property.fieldName());
    return fields;
}

QStringList MetaObject::relations() const
{
    return d->relations;
}

NamingConvention *MetaObject::namingConvention() const
{
    return NamingConvention::convention(d->namingConvention);
}

QString MetaObject::nammingConventionName() const
{
    return d->namingConvention;
}

QString MetaObject::connectionName() const
{
    return d->connectionName;
}

Connection MetaObject::connection() const
{
    return Connection::connection(d->connectionName);
}

bool MetaObject::isValid() const
{
    return d->primaryPropertyIndex >= 0 && !d->connectionName.isEmpty();
}

MetaObject MetaObject::fromQtMetaObject(const QMetaObject &metaObject)
{
    static MetaObjectGenerator generator;
    return generator.generate(metaObject, true);
}

}
