#include "metaobject.h"
#include "metaobject_p.h"

#include <QEloquent/datamap.h>
#include <QEloquent/metaproperty.h>
#include <QEloquent/metaobjectgenerator.h>
#include <QEloquent/namingconvention.h>
#include <QEloquent/connection.h>
#include <QEloquent/model.h>
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
    QList<MetaProperty> result;

    for (const MetaProperty& prop : std::as_const(d->properties)) {
        bool typeMatch;
        bool filterMatch;

        // Filter by type
        if (filters == AllProperties) {
            typeMatch = true;
        } else {
            typeMatch = false;
            if (filters.testFlag(StandardProperties) && prop.propertyType() == MetaProperty::StandardProperty)
                typeMatch = true;
            if (filters.testFlag(DynamicProperties) && prop.propertyType() == MetaProperty::DynamicProperty)
                typeMatch = true;
            if (filters.testFlag(AppendedProperties) && prop.propertyType() == MetaProperty::AppendedProperty)
                typeMatch = true;
            if (filters.testFlag(RelationProperties) && prop.propertyType() == MetaProperty::RelationProperty)
                typeMatch = true;
        }

        // Filter by attributes
        if (attributes == MetaProperty::NoAttibutes)
            filterMatch = true;
        else
            filterMatch = (prop.attributes() & attributes) != MetaProperty::NoAttibutes;

        if (typeMatch && filterMatch)
            result.append(prop);
    }

    return result;
}

QList<MetaProperty> MetaObject::properties(PropertyFilters filters) const
{
    if (filters == AllProperties) return d->properties;

    QList<MetaProperty> result;

    for (const MetaProperty& prop : std::as_const(d->properties)) {
        bool typeMatch;

        // Filter by type
        if (filters == AllProperties) {
            typeMatch = true;
        } else {
            typeMatch = false;
            if (filters.testFlag(StandardProperties) && prop.propertyType() == MetaProperty::StandardProperty)
                typeMatch = true;
            if (filters.testFlag(DynamicProperties) && prop.propertyType() == MetaProperty::DynamicProperty)
                typeMatch = true;
            if (filters.testFlag(AppendedProperties) && prop.propertyType() == MetaProperty::AppendedProperty)
                typeMatch = true;
            if (filters.testFlag(RelationProperties) && prop.propertyType() == MetaProperty::RelationProperty)
                typeMatch = true;
        }

        if (typeMatch)
            result.append(prop);
    }

    return result;
}

DataMap MetaObject::read(const Model *model,
                         MetaProperty::PropertyAttributes attributes,
                         PropertyFilters filters,
                         PropertyNameResolution resolution) const
{
    DataMap data;

    const QList<MetaProperty> properties = this->properties(attributes, filters);
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
            const QList<DataMap> maps = relation->serialize();
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
