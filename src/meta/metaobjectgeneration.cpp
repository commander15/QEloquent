#include "metaobjectgeneration_p.h"

#include "metaobject_p.h"
#include "metaproperty_p.h"

#include <QEloquent/private/debug_p.h>
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

    // Discovering properties
    discoverProperties();
    QStringList propertyNames;
    using Type = MetaProperty::PropertyType;
    for (const MetaPropertyData *prop : std::as_const(m_properties))
        if (prop->propertyType == Type::StandardProperty || prop->propertyType == Type::DynamicProperty)
            propertyNames.append(prop->propertyName);
    m_propertyAttributes = propertyAttributes(propertyNames);

    // Run pending tasks
    for (const auto &task : std::as_const(m_pendingTasks))
        task();

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

    if (data->primaryPropertyIndex < 0) {
        qelWarning() << "MetaObject: " << qtMetaObject->className() << " model didn't have a primary key";
    }

    return MetaObject(data);
}

QMap<MetaProperty::PropertyAttributeFlag, QStringList> MetaObjectGeneration::propertyAttributes(const QStringList &properties) const
{
    using Attr = MetaProperty::PropertyAttributeFlag;
    QMap<Attr, QStringList> map;

    struct Value {
        Value(const char *str) : string(str) {}
        Value(const QStringList &lst) : list(lst) {}

        QString string;
        QStringList list;
    };

    struct Entry {
        Attr attribute;
        QString info;
        Value defaultValue;
        bool list = true;
    };

    const QList<Entry> entries = {
        Entry{ Attr::PrimaryProperty, META_PRIMARY, "id", false },
        Entry{ Attr::LabelProperty, META_LABEL, "", false },
        Entry{ Attr::GeneratedProperty, META_GENERATED, "", true },
        Entry{ Attr::CreationTimestamp, META_CREATED_AT, "createdAt", false },
        Entry{ Attr::UpdateTimestamp, META_UPDATED_AT, "updatedAt", false },
        Entry{ Attr::DeletionTimestamp, META_DELETED_AT, "deletedAt", false },
        Entry{ Attr::HiddenProperty, META_HIDDEN, "", true },
        Entry{ Attr::FillableProperty, META_FILLABLE, properties, true },
        Entry{ Attr::DatabaseField, META_MAPPED, properties, true },
    };

    for (const Entry &entry : entries) {
        QStringList values;

        if (entry.list) {
            values = infoList(entry.info, entry.defaultValue.list);
        } else {
            const QString value = info(entry.info, entry.defaultValue.string);
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
    const RawInfoList result = rawInfo(name) + rawInfo(name, { META_OPERATOR_INCLUSIVE });
    return !result.isEmpty();
}

QString MetaObjectGeneration::info(const QString &name, const QString &defaultValue) const
{
    const RawInfoList raw = rawInfo(name);
    const QStringList result = (raw.isEmpty() ? QStringList() : raw.first().second);
    return (result.isEmpty() ? defaultValue : result.first());
}

QStringList MetaObjectGeneration::infoList(const QString &name, const QString &defaultValue) const
{
    QStringList def = defaultValue.split(',', Qt::SkipEmptyParts);
    for (QString &item : def)
        item = item.trimmed();
    return infoList(name, def);
}

QStringList MetaObjectGeneration::infoList(const QString &name, const QStringList &defaultValue) const
{
    auto parse = [](const QString &input) {
        QStringList output = input.split(',', Qt::SkipEmptyParts);
        for (QString &o : output)
            o = o.trimmed();
        return output;
    };

    QStringList result;

    RawInfoList raw = rawInfo(name, QStringList() << META_OPERATOR_INCLUSIVE << META_OPERATOR_EXCLUSIVE);

    if (!defaultValue.isEmpty()) {
        auto it = std::find_if(raw.begin(), raw.end(), [](const RawInfo &info) {
            return info.first.isEmpty();
        });

        if (it == raw.end())
            raw.prepend({"", defaultValue});
        else if (it->second.isEmpty())
            it->second = defaultValue;
    }

    for (const RawInfo &info : std::as_const(raw)) {
        if (info.first.isEmpty() || info.first == META_OPERATOR_INCLUSIVE) {
            for (const QString &item : info.second) {
                for (const QString &token : parse(item)) {
                    if (!result.contains(token))
                        result.append(token);
                }
            }
        } else if (info.first == META_OPERATOR_EXCLUSIVE) {
            for (const QString &item : info.second) {
                for (const QString &token : parse(item)) {
                    result.removeAll(token);
                }
            }
        }
    }

    return result;
}

MetaObjectGeneration::RawInfoList MetaObjectGeneration::rawInfo(const QString &name, const QStringList suffixes, int depth) const
{
    QList<QPair<QString, QStringList>> result;

    static const QStringList defaultSuffix = { "" };
    auto getNames = [&name](const QString &suffix) {
        QByteArrayList names;

        const QStringList topNames = name.split(",", Qt::SkipEmptyParts);
        for (const QString &topName : topNames) {
            const QByteArray baseName = (topName.trimmed() + suffix).toLatin1();
            names << "orm:" + baseName << baseName;
        }

        return names;
    };

    const QList<const QMetaObject *> metaObjects = getMetaObjects(true);
    for (const QMetaObject *metaObject : metaObjects) {
        for (const QString &suffix : defaultSuffix + suffixes) {
            const QByteArrayList names = getNames(suffix);
            QStringList values;

            for (const QByteArray &name : names) {
                int index = metaObject->indexOfClassInfo(name.constData());
                if (index < 0)
                    continue;

                QString value(metaObject->classInfo(index).value());
                value = value.trimmed();

                if (!value.isEmpty())
                    values.append(value);
            }

            if (!values.isEmpty())
                result.append({ suffix, values });
        }

        --depth;
        if (depth == 0)
            break;
    }

    return result;
}

QList<const QMetaObject *> MetaObjectGeneration::getMetaObjects(bool fromBaseToChild) const
{
    QList<const QMetaObject *> chain;
    const QMetaObject *current = qtMetaObject;

    while (current != nullptr) {
        if (fromBaseToChild) {
            chain.prepend(current);
        } else {
            chain.append(current);
        }
        current = current->superClass();
    }

    return chain;
}

void MetaObjectGeneration::discoverProperties()
{
    using Attr = MetaProperty::PropertyAttributeFlag;
    using Type = MetaProperty::PropertyType;

    QStringList potentialDynamicProperties;
    potentialDynamicProperties.append(infoList(META_DYNAMIC));
    potentialDynamicProperties.append(infoList(META_FILLABLE));
    potentialDynamicProperties.append(infoList(META_HIDDEN));
    potentialDynamicProperties.append(infoList(META_MAPPED));
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

    auto finalize = [this, name, property, propertyIndex, options, save]() {
        for (const auto &i : std::as_const(m_propertyAttributes).asKeyValueRange())
            if (i.second.contains(name))
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
            index(Attr::PrimaryProperty, data->primaryPropertyIndex);
            index(Attr::LabelProperty, data->labelPropertyIndex);
            index(Attr::CreationTimestamp, data->creationTimestampIndex);
            index(Attr::UpdateTimestamp, data->updateTimestampIndex);
            index(Attr::DeletionTimestamp, data->deletionTimestampIndex);
        }
    };

    if (save)
        m_properties.append(property);

    m_pendingTasks.append(finalize);

    return property;
}

const QString MetaObjectGeneration::s_propertyAll("__all_properties__");

} // namespace QEloquent
