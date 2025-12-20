#include "metaobjectgenerator.h"

#include <QEloquent/namingconvention.h>
#include <QEloquent/connection.h>
#include <QEloquent/metaobjectregistry.h>
#include <QEloquent/private/metaobject_p.h>
#include <QEloquent/private/metaproperty_p.h>

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

struct MetaObjectGeneration {
    MetaObjectGeneration(const QMetaObject *qtMetaObject)
        : object(new MetaObjectPrivate())
        , qtMetaObject(qtMetaObject)
        , convention(nullptr) {
        object->metaObject = qtMetaObject;
    }

    MetaObjectPrivate *object;
    const QMetaObject *qtMetaObject;

    NamingConvention *convention;
    Connection connection;

    QStringList fillable;
    QStringList hidden;

    bool hasInfo(const QString &name) const
    { return infoIndex(name) >= 0; }

    QString info(const QString &name, const QString &defaultValue = QString()) const
    {
        const int index = infoIndex(name);
        return (index < 0 ? defaultValue : QString(qtMetaObject->classInfo(index).value()));
    }

    QStringList infoList(const QString &name) const
    {
        QStringList items = info(name).split(',', Qt::SkipEmptyParts);
        for (QString &item : items)
            item = item.trimmed();
        return items;
    }

    int infoIndex(const QString &name) const
    { return qtMetaObject->indexOfClassInfo(name.toStdString().c_str()); }
};

MetaObjectGenerator::MetaObjectGenerator()
{}

MetaObject MetaObjectGenerator::generate(const QMetaObject &modelMetaObject, bool cache)
{
    // If the meta object is already registered, we just return the cached version
    const QString className(modelMetaObject.className());
    if (MetaObjectRegistry::contains(className))
        return MetaObjectRegistry::metaObject(modelMetaObject.className());

    // We generate the meta object for first time use
    MetaObjectGeneration generation(&modelMetaObject);
    initGeneration(&generation);
    discoverProperties(&generation);

    // We cache (if applicable) and return the meta object
    const MetaObject object(generation.object);
    if (cache)
        MetaObjectRegistry::registerMetaObject(object);
    return object;
}

void MetaObjectGenerator::initGeneration(MetaObjectGeneration *generation)
{
    generation->convention = (generation->hasInfo(META_NAMING) ? NamingConvention::convention(generation->info(META_NAMING)) : NamingConvention::convention());
    generation->connection = (generation->hasInfo(META_CONNECTION) ? Connection::connection(generation->info(META_CONNECTION)) : Connection::defaultConnection());

    const QString className = QString(generation->qtMetaObject->className()).section("::", -1);
    generation->object->tableName = (generation->hasInfo(META_TABLE) ? generation->info(META_TABLE) : generation->convention->tableName(className));
    generation->object->connectionName = generation->connection.name();

    generation->fillable = generation->infoList(META_FILLABLE);
    generation->hidden = generation->infoList(META_HIDDEN);
}

void MetaObjectGenerator::discoverProperties(MetaObjectGeneration *generation)
{
    // Global properties index
    int index = -1;

    // first class properties
    for (int i(0); i < generation->qtMetaObject->propertyCount(); ++i) {
        const QMetaProperty base = generation->qtMetaObject->property(i);

        MetaPropertyData *property = new MetaPropertyData();
        property->propertyName = base.name();
        property->metaType = base.metaType();
        property->metaProperty = base;
        property->metaType = base.metaType();
        tuneProperty(index, property, generation, true);
    }

    // Appended properties
    const QStringList append = generation->infoList("append");
    for (const QString &appendItem : append) {
        const int methodIndex = generation->qtMetaObject->indexOfMethod(appendItem.toStdString().c_str());
        if (methodIndex < 0)
            continue;

        const QMetaMethod method = generation->qtMetaObject->method(methodIndex);

        MetaPropertyData *property = new MetaPropertyData();
        property->propertyName = appendItem;
        property->metaType = method.returnMetaType();
        property->getter = method;
        property->propertyType = MetaProperty::AppendedProperty;
        tuneProperty(index, property, generation, true);
    }

    // Relations properties
    const QStringList with = generation->infoList("with");
    for (const QString &relation : with) {
        const int methodIndex = generation->qtMetaObject->indexOfMethod(relation.toStdString().c_str());
        if (methodIndex < 0)
            continue;

        const QMetaMethod method = generation->qtMetaObject->method(methodIndex);

        MetaPropertyData *property = new MetaPropertyData();
        property->propertyName = relation;
        property->metaType = method.returnMetaType();
        property->getter = method;
        property->propertyType = MetaProperty::RelationProperty;
        tuneProperty(index, property, generation, true);
    }
}

void MetaObjectGenerator::tuneProperty(int &index, MetaPropertyData *property, MetaObjectGeneration *generation, bool save)
{
    // We increment global index first
    if (save)
        ++index;

    // Field name: first we check if it get ovirriden by the user (META_PROPERTY_FIELD for the format), if not, we deduce using convention
    const int fieldNameIndex = generation->qtMetaObject->indexOfClassInfo(QStringLiteral(META_PROPERTY_FIELD).arg(property->propertyName).toStdString().c_str());
    if (fieldNameIndex < 0)
        property->fieldName = generation->convention->fieldName(property->propertyName, generation->object->tableName);
    else
        property->fieldName = generation->qtMetaObject->classInfo(fieldNameIndex).value();

    if (property->propertyName == generation->info(META_PRIMARY, "id")) {
        property->attributes.setFlag(MetaProperty::PrimaryProperty);
        generation->object->primaryPropertyIndex = index;

        // We generate foreign property name if not provided, camel cased
        // such as studentId is from 'Student' class and 'id' primary property
        if (!generation->object->foreignProperty.isValid()) {
            const QString className = QString(generation->qtMetaObject->className()).section("::", -1);
            MetaPropertyData *foreign = new MetaPropertyData();
            foreign->propertyName = generation->convention->foreignPropertyName(property->propertyName, className);
            foreign->metaType = property->metaType;
            tuneProperty(index, foreign, generation, false);
            generation->object->foreignProperty = MetaProperty(foreign);
        }
    }

    if (property->propertyName == generation->info(META_CREATED_AT, "createdAt")) {
        property->attributes.setFlag(MetaProperty::CreationTimestamp);
        generation->object->creationTimestampIndex = index;
    }

    if (property->propertyName == generation->info(META_UPDATED_AT, "updatedAt")) {
        property->attributes.setFlag(MetaProperty::UpdateTimestamp);
        generation->object->updateTimestampIndex = index;
    }

    if (property->propertyName == generation->info(META_DELETED_AT, "deletedAt")) {
        property->attributes.setFlag(MetaProperty::CreationTimestamp);
        generation->object->deletionTimestampIndex = index;
    }

    if (!generation->hasInfo(META_FILLABLE) || generation->fillable.contains(property->propertyName))
        property->attributes.setFlag(MetaProperty::FillableProperty);

    if (generation->hidden.contains(property->propertyName))
        property->attributes.setFlag(MetaProperty::HiddenProperty);

    // Registering property
    if (save)
        generation->object->properties.append(MetaProperty(property));
}

} // namespace QEloquent
