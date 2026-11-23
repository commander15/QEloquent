#include "metaobjectgeneration_p.h"

#include "metaobject_p.h"
#include "metaproperty_p.h"

#include <QEloquent/namingconvention.h>

#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

namespace QEloquent {

MetaObjectGeneration::MetaObjectGeneration(const QMetaObject *qtMetaObject)
    : data(nullptr)
    , qtMetaObject(qtMetaObject)
{}

MetaObject MetaObjectGeneration::generate()
{
    const QString className = QString(qtMetaObject->className()).section("::", -1);

    data = new MetaObjectPrivate();
    data->namingConvention = info(META_NAMING, "Laravel");
    data->connectionName = info(META_CONNECTION);
    data->metaObject = qtMetaObject;

    NamingConvention *convention = NamingConvention::convention(data->namingConvention);

    const QString defaultTableName = convention->tableName(className);
    data->tableName = info(META_TABLE, defaultTableName);

    QList<const QMetaObject *> metaObjects;
    const QMetaObject *metaO = qtMetaObject;
    do {
        metaObjects.prepend(metaO);
        metaO = metaO->superClass();
    } while (metaO != nullptr);

    for (const QMetaObject *metaObject : std::as_const(metaObjects)) {
        MetaObjectContext context(metaObject);
        context;
    }

    std::transform(m_properties.begin(), m_properties.end(), std::back_inserter(data->properties), [this, &className, &convention](MetaPropertyData *d) {
        if (d->attributes.testFlag(MetaProperty::PrimaryProperty)) {
            QString name = info(META_FOREIGN, convention->foreignPropertyName(d->propertyName, className));
            MetaPropertyData *foreign = addProperty(name, d->propertyType, { .metaType = d->metaType, .mappable = false, .attributes = d->attributes }, false);
            foreign->metaProperty = d->metaProperty;
            foreign->getter = d->getter;
            foreign->setter = d->setter;
            data->foreignProperty = MetaProperty(foreign);
        }

        return MetaProperty(d);
    });

    return MetaObject(data);
}

QMap<MetaProperty::PropertyAttributeFlag, QStringList> MetaObjectGeneration::propertyAttributes() const
{
    using Attr = MetaProperty::PropertyAttributeFlag;
    QMap<Attr, QStringList> map;

    MetaObjectContext context(qtMetaObject);
    context.foreachProperty([&map](const MetaObjectContext::Prop &prop) {
        if (prop.attribs.testFlag(Attr::PrimaryProperty))
            map[Attr::PrimaryProperty].append(prop.name);
    });

    struct Entry {
        Attr attribute;
        QString info;
        QString defaultValue;
        bool list = true;
    };

    const QStringList allProperties = context.propertyNames();

    const QList<Entry> entries = {
        Entry{ Attr::PrimaryProperty, META_PRIMARY, "id", false },
        Entry{ Attr::LabelProperty, META_LABEL, context.userPropertyName(), false },
        Entry{ Attr::GeneratedProperty, META_GENERATED, "", true },
        Entry{ Attr::CreationTimestamp, META_CREATED_AT, "createdAt", false },
        Entry{ Attr::UpdateTimestamp, META_UPDATED_AT, "updatedAt", false },
        Entry{ Attr::DeletionTimestamp, META_DELETED_AT, "deletedAt", false },
        Entry{ Attr::HiddenProperty, META_HIDDEN, "", true },
        Entry{ Attr::FillableProperty, META_FILLABLE, allProperties.join(","), true },
        Entry{ Attr::DatabaseField, META_MAPPED, s_propertyAll, true },
    };

    for (const Entry &entry : entries) {
        QStringList values;

        if (entry.list) {
            values = context.infoList(entry.info, entry.defaultValue);
        } else {
            const QString value = context.info(entry.info, entry.defaultValue);
            if (!value.isEmpty())
                values.append(value);
        }

        if (!values.isEmpty())
            map.insert(entry.attribute, values);
    }

    return map;
}

bool MetaObjectGeneration::hasInfo(const QString &name) const
{
    const QStringList result = rawInfo(name);
    return !result.isEmpty();
}

QString MetaObjectGeneration::info(const QString &name, const QString &defaultValue) const
{
    const QStringList result = rawInfo(name);
    return (result.isEmpty() ? defaultValue : result.first());
}

QStringList MetaObjectGeneration::infoList(const QString &name, const QString &defaultValue) const
{
    auto parse = [](const QString &input) {
        QStringList output = input.split(',', Qt::SkipEmptyParts);
        for (QString &o : output)
            o = o.trimmed();
        return output;
    };

    const QStringList result = rawInfo(name);
    if (result.isEmpty())
        return parse(defaultValue);

    QStringList values;

    for (const QString &res : result) {
        const QStringList list = parse(res);
        if (!list.isEmpty())
            values.append(list);
    }

    values.removeDuplicates();
    return values;
}

QStringList MetaObjectGeneration::rawInfo(const QString &name, int depth) const
{
    QStringList result;

    QByteArrayList names;
    {
        const QStringList infoNames = name.split(',', Qt::SkipEmptyParts);
        for (const QString &iName : infoNames) {
            names.append(("orm:" + iName.trimmed()).toLatin1());
            names.append(iName.trimmed().toLatin1());
        }
    }


    const QMetaObject *metaObject = qtMetaObject;

    do {
        for (const QByteArray &name : names) {
            int index = metaObject->indexOfClassInfo(name.constData());
            if (index < 0)
                continue;

            QString value(metaObject->classInfo(index).value());
            value = value.trimmed();
            if (!value.isEmpty())
                result.append(value);
        }

        metaObject = metaObject->superClass();
        --depth;
    } while (metaObject != nullptr && depth > 0);

    return result;
}

void MetaObjectGeneration::discoverProperties()
{
    using Attr = MetaProperty::PropertyAttributeFlag;
    using Type = MetaProperty::PropertyType;

    QStringList potentialDynamicProperties;
    potentialDynamicProperties.append(infoList(META_DYNAMIC));
    potentialDynamicProperties.append(m_propertyAttributes[Attr::GeneratedProperty]);
    potentialDynamicProperties.append(m_propertyAttributes[Attr::FillableProperty]);
    potentialDynamicProperties.append(m_propertyAttributes[Attr::HiddenProperty]);
    potentialDynamicProperties.append(m_propertyAttributes[Attr::DatabaseField]);
    potentialDynamicProperties.removeAll(s_propertyAll);
    potentialDynamicProperties.removeDuplicates();

    // Standard properties
    for (int i(0); i < qtMetaObject->propertyCount(); ++i) {
        const QMetaProperty property = qtMetaObject->property(i);
        PropertyOptions options = { .metaType = property.metaType(), .mappable = true };
        if (property.isUser())
            options.attributes.setFlag(Attr::LabelProperty, true);

        MetaPropertyData *prop = addProperty(property.name(), Type::StandardProperty, options);
        prop->metaProperty = property;

        potentialDynamicProperties.removeOne(prop->propertyName);
    }

    // Computed/Relation properties
    for (int i(0); i < qtMetaObject->methodCount(); ++i) {
        const QMetaMethod method = qtMetaObject->method(i);
        PropertyOptions options = { .metaType = method.returnMetaType(), .mappable = false };

        Type type;
        QString returnType(method.returnMetaType().name());
        if (returnType.contains("Relation<"))
            type = Type::RelationProperty;
        else
            type = Type::ComputedProperty;

        // Only explicitly set computed properties can be accepted
        if (type == Type::ComputedProperty) {
            if (!infoList(META_COMPUTED).contains(method.name()))
                continue;
        }

        MetaPropertyData *prop = addProperty(method.name(), type, options);
        prop->getter = method;
        prop->attributes.setFlag(Attr::FillableProperty, false);

        potentialDynamicProperties.removeOne(prop->propertyName);
    }

    // Dynamic properties
    for (const QString &name : std::as_const(potentialDynamicProperties)) {
        PropertyOptions options = { .metaType = QMetaType::fromType<QVariant>(), .mappable = true };
        MetaPropertyData *prop = addProperty(name, Type::DynamicProperty, options);
        Q_UNUSED(prop)
    }
}

MetaPropertyData *MetaObjectGeneration::addProperty(const QString &name, MetaProperty::PropertyType type, const PropertyOptions &options, bool save)
{
    // index
    int propertyIndex = m_properties.count();

    MetaPropertyData *property = new MetaPropertyData();
    property->propertyName = name;
    property->metaType = options.metaType;
    property->propertyType = type;
    property->attributes = options.attributes;

    // Field name
    NamingConvention *convention = NamingConvention::convention(data->namingConvention);
    property->fieldName = info(QString(META_PROPERTY_FIELD).arg(name), convention->fieldName(name, data->tableName));

    using Attr = MetaProperty::PropertyAttributeFlag;

    for (const auto &i : std::as_const(m_propertyAttributes).asKeyValueRange())
        if (i.second.contains(s_propertyAll) || i.second.contains(name))
            property->attributes.setFlag(i.first, true);

    auto is = [&property](Attr a) { return property->attributes.testFlag(a); };
    auto set = [&property](Attr a, bool on) { property->attributes.setFlag(a, on); };
    auto index = [&property, &propertyIndex](Attr a, int &index) {
        if (property->attributes.testAnyFlag(a))
            index = propertyIndex;
    };

    if (is(Attr::PrimaryProperty)) {
        set(Attr::PrimaryProperty, true);
        set(Attr::GeneratedProperty, true);
        set(Attr::FillableProperty, false);
    }

    if (is(Attr::CreationTimestamp)) {
        set(Attr::FillableProperty, false);
        set(Attr::GeneratedProperty, false);
    }

    if (is(Attr::UpdateTimestamp)) {
        set(Attr::FillableProperty, false);
        set(Attr::GeneratedProperty, false);
    }

    if (is(Attr::DeletionTimestamp)) {
        set(Attr::FillableProperty, false);
        set(Attr::GeneratedProperty, false);
    }

    if (!options.mappable)
        set(Attr::DatabaseField, false);

    if (save) {
        m_properties.append(property);

        index(Attr::PrimaryProperty, data->primaryPropertyIndex);
        index(Attr::LabelProperty, data->labelPropertyIndex);
        index(Attr::CreationTimestamp, data->creationTimestampIndex);
        index(Attr::UpdateTimestamp, data->updateTimestampIndex);
        index(Attr::DeletionTimestamp, data->deletionTimestampIndex);
    }

    return property;
}

const QString MetaObjectGeneration::s_propertyAll("__all_properties__");

MetaObjectContext::MetaObjectContext(const QMetaObject *metaObject)
    : m_metaObject(metaObject)
{
    QStringList standard;
    QStringList computed;
    QStringList relations;

    for (int i(metaObject->propertyOffset()); i < metaObject->propertyCount(); ++i) {
        const QMetaProperty prop = metaObject->property(i);
        standard.append(prop.name());
        m_metaTypes.insert(prop.name(), prop.metaType());
    }

    const QStringList computedList = infoList(META_COMPUTED, "__all__");
    for (int i(metaObject->methodOffset()); i < metaObject->methodCount(); ++i) {
        const QMetaMethod method = metaObject->method(i);
        const QString type(method.returnMetaType().name());

        if (type.contains("Relation<"))
            relations.append(method.name());
        else if (computedList.contains("__all__") || computedList.contains(method.name()))
            computed.append(method.name());
        else
            continue;

        m_metaTypes.insert(method.name(), method.returnMetaType());
    }

    if (!standard.isEmpty())
        m_properties.insert(MetaProperty::StandardProperty, standard);

    if (!computed.isEmpty())
        m_properties.insert(MetaProperty::ComputedProperty, computed);

    if (!relations.isEmpty())
        m_properties.insert(MetaProperty::RelationProperty, relations);
}

QString MetaObjectContext::primaryProperty() const
{
    if (hasInfo(META_PRIMARY))
        return info(META_PRIMARY);

    if (!m_properties[MetaProperty::StandardProperty].isEmpty())
        return m_properties[MetaProperty::StandardProperty].constFirst();

    return QString();
}

QString MetaObjectContext::labelProperty() const
{
    if (hasInfo(META_LABEL))
        return info(META_LABEL);

    QMetaProperty userProp = m_metaObject->userProperty();
    if (userProp.isValid())
        return userProp.name();

    if (m_properties[MetaProperty::StandardProperty].count() > 1)
        return m_properties[MetaProperty::StandardProperty].at(1);

    return QString();
}

QStringList MetaObjectContext::generatedProperties() const
{
    return infoList(META_GENERATED);
}

QStringList MetaObjectContext::fillableProperties() const
{
    if (hasInfo(META_FILLABLE))
        return infoList(META_FILLABLE);

    QStringList props;
    props.append(m_properties[MetaProperty::StandardProperty]);
    props.append(m_properties[MetaProperty::DynamicProperty]);
    return props;
}

QStringList MetaObjectContext::hiddenProperties() const
{
    return infoList(META_HIDDEN);
}

QStringList MetaObjectContext::mappedProperties() const
{
    if (hasInfo(META_MAPPED))
        return infoList(META_MAPPED);

    QStringList props;
    props.append(m_properties[MetaProperty::StandardProperty]);
    props.append(m_properties[MetaProperty::DynamicProperty]);
    return props;
}

QStringList MetaObjectContext::typedProperties(MetaProperty::PropertyType type) const
{
    if (type != MetaProperty::DynamicProperty)
        return m_properties[type];

    QStringList props = infoList(META_DYNAMIC);
    {
        QStringList lookIn;
        lookIn << META_PRIMARY << META_LABEL << META_GENERATED
               << META_CREATED_AT << META_UPDATED_AT << META_DELETED_AT
               << META_MAPPED << META_FILLABLE << META_HIDDEN;

        for (const QString &info : std::as_const(lookIn)) {
            const QStringList values = infoList(info);
            if (!values.isEmpty())
                props.append(values);
        }
    }

    props.removeIf([this](const QString &prop) {
        bool standard = m_properties[MetaProperty::StandardProperty].contains(prop);
        bool computed = m_properties[MetaProperty::ComputedProperty].contains(prop);
        bool relation = m_properties[MetaProperty::RelationProperty].contains(prop);
        return standard || computed || relation;
    });

    props.removeDuplicates();
    return props;
}

void MetaObjectContext::foreachProperty(std::function<void(const Prop &)> callback)
{
    using Attr = MetaProperty::PropertyAttributeFlag;
    using Type = MetaProperty::PropertyType;

    auto loop = [this, &callback](Type t) {
        const QStringList props = typedProperties(t);
        for (const QString &propName : props) {
            MetaProperty::PropertyAttributes attribs;
            auto attrib = [&propName, this, &attribs](Attr a, bool o) {
                if (o)
                    attribs.setFlag(a, true);
            };

            attrib(Attr::PrimaryProperty, propName == primaryProperty());
            attrib(Attr::LabelProperty, propName == labelProperty());
            attrib(Attr::GeneratedProperty, generatedProperties().contains(propName));
            attrib(Attr::CreationTimestamp, propName == info(META_CREATED_AT, "createdAt"));
            attrib(Attr::UpdateTimestamp, propName == info(META_UPDATED_AT, "updatedAt"));
            attrib(Attr::DeletionTimestamp, propName == info(META_DELETED_AT, "deletedAt"));
            attrib(Attr::FillableProperty, fillableProperties().contains(propName));
            attrib(Attr::HiddenProperty, hiddenProperties().contains(propName));
            attrib(Attr::DatabaseField, mappedProperties().contains(propName));

            const QMetaType metaType = m_metaTypes.value(propName, QMetaType::fromType<QVariant>());
            callback({ .name = propName, .attribs = attribs, .type = t, .metaType = metaType });
        }
    };

    loop(MetaProperty::StandardProperty);
    loop(MetaProperty::DynamicProperty);
    loop(MetaProperty::ComputedProperty);
    loop(MetaProperty::RelationProperty);
}

bool MetaObjectContext::hasInfo(const QString &name) const
{
    const QStringList result = rawInfo(name);
    return !result.isEmpty();
}

QString MetaObjectContext::info(const QString &name, const QString &defaultValue) const
{
    const QStringList result = rawInfo(name);
    return (result.isEmpty() ? defaultValue : result.first());
}

QStringList MetaObjectContext::infoList(const QString &name, const QString &defaultValue) const
{
    auto parse = [](const QString &input) {
        QStringList output = input.split(',', Qt::SkipEmptyParts);
        for (QString &o : output)
            o = o.trimmed();
        return output;
    };

    const QStringList result = rawInfo(name);
    if (result.isEmpty())
        return parse(defaultValue);

    QStringList values;

    for (const QString &res : result) {
        const QStringList list = parse(res);
        if (!list.isEmpty())
            values.append(list);
    }

    values.removeDuplicates();
    return values;
}

QStringList MetaObjectContext::infoList(const QString &name, const QStringList &defaultValue) const
{
    return infoList(name, defaultValue.join(','));
}

QStringList MetaObjectContext::rawInfo(const QString &name) const
{
    QStringList result;

    QByteArrayList names;
    {
        const QStringList infoNames = name.split(',', Qt::SkipEmptyParts);
        for (const QString &iName : infoNames) {
            names.append(("orm:" + iName.trimmed()).toLatin1());
            names.append(iName.trimmed().toLatin1());
        }
    }

    for (const QByteArray &name : names) {
        int index = m_metaObject->indexOfClassInfo(name.constData());
        if (index < 0 || index < m_metaObject->classInfoOffset())
            continue;

        QString value(m_metaObject->classInfo(index).value());
        value = value.trimmed();
        if (!value.isEmpty())
            result.append(value);
    }

    return result;
}

} // namespace QEloquent
