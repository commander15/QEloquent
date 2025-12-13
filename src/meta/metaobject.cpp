#include "metaobject.h"
#include "metaobject_p.h"

#include <QEloquent/metaproperty.h>
#include <QEloquent/metaobjectgenerator.h>
#include <QEloquent/connection.h>
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
    if (d->primaryPropertyIndex < d->properties.size())
        return d->properties.at(d->primaryPropertyIndex);
    else
        return MetaProperty();
}

MetaProperty MetaObject::foreignProperty() const
{
    return d->foreignProperty;
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

QList<MetaProperty> MetaObject::properties(MetaProperty::PropertyAttributes attributes, PropertyFilters filters) const
{
    QList<MetaProperty> properties = d->properties;

    properties.removeIf([&attributes, &filters](const MetaProperty &property) {
        static const QList<MetaProperty::PropertyAttributeFlag> allAttributes = {
            MetaProperty::PrimaryProperty,
            MetaProperty::CreationTimestamp,
            MetaProperty::UpdateTimestamp,
            MetaProperty::DeletionTimestamp,
            MetaProperty::FillableProperty,
            MetaProperty::HiddenProperty
        };

        static const QMap<PropertyFilterFlag, MetaProperty::PropertyType> allTypes = {
            { StandardProperties, MetaProperty::StandardProperty },
            { AppendedProperties, MetaProperty::AppendedProperty },
            { RelationProperties, MetaProperty::RelationProperty }
        };

        const QList<PropertyFilterFlag> allFilters = allTypes.keys();
        for (PropertyFilterFlag filter : allFilters) {
            MetaProperty::PropertyType propertyType = allTypes.value(filter);
            if (!filters.testFlag(filter) && property.propertyType() == propertyType)
                return true;
        }

        for (MetaProperty::PropertyAttributeFlag attribute : allAttributes)
            if (attributes.testFlag(attribute) && property.hasAttribute(attribute))
                return false;

        return true;
    });

    return properties;
}

QList<MetaProperty> MetaObject::properties() const
{
    return d->properties;
}

QVariantMap MetaObject::readProperties(const Model *model, MetaProperty::PropertyAttributes attributes, PropertyFilters filters, PropertyNameResolution resolution) const
{
    QVariantMap data;

    const QList<MetaProperty> properties = this->properties(attributes, filters);
    for (const MetaProperty &property : properties) {
        switch (resolution) {
        case ResolveByPropertyName:
            data.insert(property.propertyName(), property.read(model));
            break;

        case ResolveByFieldName:
            data.insert(property.fieldName(), property.read(model));
            break;
        }
    }

    return data;
}

int MetaObject::writeProperties(Model *model, const QVariantMap &data, PropertyNameResolution resolution) const
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

QVariantMap MetaObject::readFillableFields(const Model *model) const
{
    return readProperties(model, MetaProperty::FillableProperty, AllProperties, ResolveByFieldName);
}

bool MetaObject::writeFillableFields(Model *model, const QVariantMap &data)
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
    return QStringList();
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
    return d->primaryPropertyIndex > 0 && !d->connectionName.isEmpty();
}

MetaObject MetaObject::fromQtMetaObject(const QMetaObject &metaObject)
{
    static MetaObjectGenerator generator;
    return generator.generate(metaObject);
}

}
