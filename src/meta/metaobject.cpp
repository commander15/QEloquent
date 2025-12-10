#include "metaobject.h"
#include "metaobject_p.h"

#include <QEloquent/namingconvention.h>
#include <QEloquent/connection.h>
#include <QEloquent/metaproperty.h>
#include <QEloquent/private/metaproperty_p.h>

#include <QMetaObject>
#include <QMetaProperty>

#define META_TABLE          "table"
#define META_PRIMARY        "primary"
#define META_FOREIGN        "foreign"
#define META_CREATED_AT     "createdAt"
#define META_UPDATED_AT     "updatedAt"
#define META_DELETED_AT     "deletedAt"
#define META_FILLABLE       "fillable"
#define META_HIDDEN         "hidden"
#define META_PROPERTY_FIELD "%1Field"
#define META_WITH           "with"
#define META_NAMING         "naming"
#define META_CONNECTION     "connection"

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
    return d->primaryProperty;
}

MetaProperty MetaObject::foreignProperty() const
{
    return d->foreignProperty;
}

bool MetaObject::hasCreationTimestamp() const
{
    return d->creationTimestamp.isValid();
}

MetaProperty MetaObject::creationTimestamp() const
{
    return d->creationTimestamp;
}

bool MetaObject::hasUpdateTimestamp() const
{
    return d->updateTimestamp.isValid();
}

MetaProperty MetaObject::updateTimestamp() const
{
    return d->updateTimestamp;
}

bool MetaObject::hasDeletionTimestamp() const
{
    return d->deletionTimestamp.isValid();
}

MetaProperty MetaObject::deletionTimestamp() const
{
    return d->deletionTimestamp;
}

MetaProperty MetaObject::property(const QString &name, PropertyResolution resolution) const
{
    return d->property(name);
}

QList<MetaProperty> MetaObject::properties() const
{
    return d->properties;
}

QVariantMap MetaObject::readFillableFields(const Model *model) const
{
    QVariantMap data;
    for (const MetaProperty &property : std::as_const(d->properties))
        if (property.hasAttribute(MetaProperty::FillableProperty))
            data.insert(property.fieldName(), property.read(model));
    return data;
}

QVariantMap MetaObject::readAllFields(const Model *model, bool excludePrimary) const
{
    QVariantMap data;
    for (const MetaProperty &property : std::as_const(d->properties))
        if (!(excludePrimary && property == d->primaryProperty))
            data.insert(property.fieldName(), property.read(model));
    return data;
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
    return d->primaryProperty.isValid() && !d->connectionName.isEmpty();
}

MetaObject MetaObject::fromQtMetaObject(const QMetaObject &metaObject)
{
    auto hasInfo = [&metaObject](const QString &name) {
        return metaObject.indexOfClassInfo(name.toStdString().c_str()) >= 0;
    };

    auto info = [&metaObject](const QString &name, const QString &defaultValue = QString()) -> QString {
        const int index = metaObject.indexOfClassInfo(name.toStdString().c_str());
        return (index < 0 ? defaultValue : QString(metaObject.classInfo(index).value()));
    };

    auto infoList = [&metaObject](const QString &name) -> QStringList
    {
        const int index = metaObject.indexOfClassInfo(name.toStdString().c_str());
        if (index < 0)
            return QStringList();

        QStringList values = QString(metaObject.classInfo(index).value()).split(",", Qt::SkipEmptyParts);
        for (QString &value : values)
            value = value.trimmed();
        return values;
    };

    const QString conventionName = info(META_NAMING, QStringLiteral("Laravel"));
    NamingConvention *convention = NamingConvention::convention(conventionName);
    Connection connection = Connection::connection(info(META_CONNECTION, Connection::defaultConnection().name()));

    const QString className = QString(metaObject.className()).section("::", -1);
    const QStringList fillable = infoList(META_FILLABLE);
    const QStringList hidden = infoList(META_HIDDEN);
    const QStringList with = infoList(META_WITH);

    MetaObjectPrivate *d = new MetaObjectPrivate();
    d->tableName = info(META_TABLE, convention->tableName(className));
    d->connectionName = connection.name();
    d->metaObject = &metaObject;

    // first class properties
    for (int i(0); i < metaObject.propertyCount(); ++i) {
        const QMetaProperty base = metaObject.property(i);

        MetaPropertyData *property = new MetaPropertyData();

        // Property name: same as Qt one
        property->propertyName = base.name();

        // Property type: same as Qt one
        property->metaType = base.metaType();

        // We need the Qt meta property too
        property->metaProperty = base;

        // Now we get type
        property->metaType = base.metaType();

        d->properties.append(MetaProperty(property));
    }

    // Appended properties
    const QStringList append = infoList("append");
    for (const QString &appendItem : append) {
        const int methodIndex = metaObject.indexOfMethod(appendItem.toStdString().c_str());
        const QMetaMethod method = metaObject.method(methodIndex);

        MetaPropertyData *data = new MetaPropertyData();
        data->propertyName = appendItem;
        data->metaType = method.returnMetaType();
        data->getter = method;
        data->propertyType = MetaProperty::AppendedProperty;
        d->properties.append(MetaProperty(data));
    }

    // Relations properties
    for (const QString &relation : with) {
        const int index = metaObject.indexOfMethod(relation.toStdString().c_str());
        if (index < 0)
            continue;

        const QMetaMethod method = metaObject.method(index);

        MetaPropertyData *data = new MetaPropertyData();
        data->propertyName = relation;
        data->metaType = method.returnMetaType();
        data->getter = method;
        data->propertyType = MetaProperty::RelationProperty;
        d->properties.append(MetaProperty(data));
    }

    // Tuning properties
    for (MetaProperty &prop : d->properties) {
        MetaPropertyData *property = prop.data.get();

        // Field name: first we check if it get ovirriden by the user (META_PROPERTY_FIELD for the format), if not, we deduce using convention
        const int fieldNameIndex = metaObject.indexOfClassInfo(QStringLiteral(META_PROPERTY_FIELD).arg(property->propertyName).toStdString().c_str());
        if (fieldNameIndex < 0)
            property->fieldName = metaObject.classInfo(fieldNameIndex).value();
        else
            property->fieldName = convention->fieldName(property->propertyName, d->tableName);

        if (property->propertyName == info(META_PRIMARY, "id")) {
            property->attributes.setFlag(MetaProperty::PrimaryProperty);
            d->primaryProperty = prop;

            MetaPropertyData *foreignData = new MetaPropertyData();
            foreignData->propertyName = convention->foreignPropertyName(property->fieldName, d->tableName);
            foreignData->fieldName = convention->foreignFieldName(property->fieldName, d->tableName);
            foreignData->metaType = property->metaType;
            d->foreignProperty = MetaProperty(foreignData);
        }

        if (property->propertyName == info(META_CREATED_AT, "createdAt"))
            property->attributes.setFlag(MetaProperty::CreationTimestamp);

        if (property->propertyName == info(META_UPDATED_AT, "updatedAt"))
            property->attributes.setFlag(MetaProperty::UpdateTimestamp);

        if (property->propertyName == info(META_DELETED_AT, "deletedAt"))
            property->attributes.setFlag(MetaProperty::CreationTimestamp);

        if (!hasInfo(META_FILLABLE) || fillable.contains(property->propertyName))
            property->attributes.setFlag(MetaProperty::FillableProperty);

        if (hidden.contains(property->propertyName))
            property->attributes.setFlag(MetaProperty::HiddenProperty);
    }

    return MetaObject(d);
}

}
