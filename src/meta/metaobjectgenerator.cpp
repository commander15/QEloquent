#include "metaobjectgenerator.h"

#include <QEloquent/namingconvention.h>
#include <QEloquent/connection.h>
#include <QEloquent/metaobjectregistry.h>
#include <QEloquent/private/metaobject_p.h>
#include <QEloquent/private/metaproperty_p.h>

#include <QSqlDatabase>
#include <QSqlRecord>

#define META_TABLE          "table"
#define META_PRIMARY        "primary"
#define META_LABEL          "label"
#define META_FOREIGN        "foreign"
#define META_CREATED_AT     "createdAt"
#define META_UPDATED_AT     "updatedAt"
#define META_DELETED_AT     "deletedAt"
#define META_FILLABLE       "fillable"
#define META_HIDDEN         "hidden"
#define META_APPEND         "append"
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

    QStringList append;
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
    const QString className(modelMetaObject.className());

    // If it's Model, we just return an invalid object
    if (className == "QEloquent::Model") {
        qWarning() << "MetaObjectGenerator: tried to register an invalid model !";
        return MetaObject();
    }

    // If the meta object is already registered, we just return the cached version
    if (cache && MetaObjectRegistry::contains(className))
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

bool MetaObjectGenerator::initGeneration(MetaObjectGeneration *generation)
{
    generation->convention = (generation->hasInfo(META_NAMING) ? NamingConvention::convention(generation->info(META_NAMING)) : NamingConvention::convention());
    generation->connection = (generation->hasInfo(META_CONNECTION) ? Connection::connection(generation->info(META_CONNECTION)) : Connection::defaultConnection());

    const QString className = QString(generation->qtMetaObject->className()).section("::", -1);
    generation->object->tableName = (generation->hasInfo(META_TABLE) ? generation->info(META_TABLE) : generation->convention->tableName(className));
    generation->object->connectionName = generation->connection.name();
    generation->object->relations = generation->infoList("with");

    if (generation->connection.isOpen()) {
        const QSqlRecord record = generation->connection.database().record(generation->object->tableName);
        if (record.isEmpty()) {
            qWarning().noquote().nospace()
                    << "MetaObjectGenerator: can't check if " << className
                << " has an actual database table named '" << generation->object->tableName << '\'';
        }
    }

    generation->append = generation->infoList(META_APPEND);
    generation->fillable = generation->infoList(META_FILLABLE);
    generation->hidden = generation->infoList(META_HIDDEN);

    return true;
}

void MetaObjectGenerator::discoverProperties(MetaObjectGeneration *generation)
{
    // first class properties
    QList<MetaPropertyData *> firstClass;
    for (int i(0); i < generation->qtMetaObject->propertyCount(); ++i) {
        const QMetaProperty base = generation->qtMetaObject->property(i);

        MetaPropertyData *property = new MetaPropertyData();
        property->propertyName = base.name();
        property->propertyType = MetaProperty::StandardProperty;
        property->metaProperty = base;
        property->metaType = base.metaType();
        property->attributes.setFlag(MetaProperty::FillableProperty, base.isWritable());
        property->attributes.setFlag(MetaProperty::DatabaseField, base.isWritable());

        if (base.isUser()) {
            property->attributes.setFlag(MetaProperty::LabelProperty, true);
            generation->object->labelPropertyIndex = i;
        }

        firstClass.append(property);
    }

    // Appended and relations properties
    QList<MetaPropertyData *> append;
    QList<MetaPropertyData *> relations;
    for (int i(0); i < generation->qtMetaObject->methodCount(); ++i) {
        const QMetaMethod method = generation->qtMetaObject->method(i);
        const QByteArray methodName(method.name());

        if (generation->append.contains(methodName)) {
            MetaPropertyData *property = new MetaPropertyData();
            property->propertyName = method.name();
            property->propertyType = MetaProperty::AppendedProperty;
            property->getter = method;
            property->metaType = method.returnMetaType();
            property->attributes = MetaProperty::NoAttibutes;
            append.append(property);
            continue;
        }

        const QByteArray typeName(method.returnMetaType().name());
        if (typeName.startsWith("QEloquent::Relation<") || typeName.startsWith("Relation<")) {
            MetaPropertyData *property = new MetaPropertyData();
            property->propertyName = method.name();
            property->propertyType = MetaProperty::RelationProperty;
            property->getter = method;
            property->metaType = method.returnMetaType();
            property->attributes = MetaProperty::NoAttibutes;
            relations.append(property);
            continue;
        }
    }

    // Dynamic properties
    // potential dynamic properties (from fillable, hidden, ...)
    QStringList potentialDynamicProperties = generation->fillable + generation->hidden;
    potentialDynamicProperties.removeDuplicates();
    potentialDynamicProperties.removeIf([&firstClass, &append, &relations](const QString &propertyName) {
        auto checker = [&propertyName](MetaPropertyData *property) {
            return property->propertyName == propertyName;
        };

        auto found = std::find_if(firstClass.begin(), firstClass.end(), checker);
        if (found != firstClass.end()) return true;

        found = std::find_if(append.begin(), append.end(), checker);
        if (found != append.end()) return true;

        found = std::find_if(relations.begin(), relations.end(), checker);
        if (found != relations.end()) return true;

        return false;
    });

    QList<MetaPropertyData *> dynamicProperties;
    for (const QString &propertyName : std::as_const(potentialDynamicProperties)) {
        MetaPropertyData *property = new MetaPropertyData();
        property->propertyName = propertyName;
        property->propertyType = MetaProperty::DynamicProperty;
        property->metaType = QMetaType::fromType<QVariant>();
        property->attributes = MetaProperty::FillableProperty | MetaProperty::DatabaseField;
        dynamicProperties.append(property);
    }

    // Global properties index
    int index = -1;

    const QList<MetaPropertyData *> allProperties = firstClass + append + relations + dynamicProperties;
    for (MetaPropertyData *property : allProperties)
        tuneProperty(index, property, generation, true);
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

    // Fillable default if no fillable directives set or if the property has been marked explicitly
    if (property->attributes.testFlag(MetaProperty::FillableProperty)) {
        if (generation->hasInfo(META_FILLABLE) && !generation->fillable.contains(property->propertyName))
            property->attributes.setFlag(MetaProperty::FillableProperty, false);
    }

    // Hidden if explicitly set
    if (generation->hidden.contains(property->propertyName))
        property->attributes.setFlag(MetaProperty::HiddenProperty, true);

    // Is DB field ?
    if (property->attributes.testFlag(MetaProperty::DatabaseField)) {
        const Connection &conn = generation->connection;
        static const QString errorStrPrefixTemplate = "MetaObjectGenerator: can't check if %1::%2 has an actual database field named '%3', ";
        const QString errorStrPrefix = errorStrPrefixTemplate
                                           .arg(generation->qtMetaObject->className(),
                                                property->propertyName,
                                                property->fieldName);

        if (conn.isOpen()) {
            const QSqlRecord record = conn.database().record(generation->object->tableName);
            if (!record.isEmpty()) {
                const int index = record.indexOf(property->fieldName);
                if (index < 0)
                    property->attributes.setFlag(MetaProperty::DatabaseField, false);
            } else {
                qDebug() .noquote().nospace() << errorStrPrefix << "database table '" << generation->object->tableName << "' not found.";
            }
        } else {
            qDebug().noquote().nospace() << "connection '" << conn.name() << "' is down.";
        }
    }

    // Is it primary ?
    if (property->propertyName == generation->info(META_PRIMARY, "id")) {
        property->attributes.setFlag(MetaProperty::PrimaryProperty, true);
        property->attributes.setFlag(MetaProperty::FillableProperty, false);
        generation->object->primaryPropertyIndex = index;

        // We generate foreign property name if not provided, camel cased
        // such as studentId is from 'Student' class and 'id' primary property
        if (!generation->object->foreignProperty.isValid()) {
            const QString className = QString(generation->qtMetaObject->className()).section("::", -1);
            MetaPropertyData *foreign = new MetaPropertyData();
            foreign->propertyName = generation->info(META_FOREIGN, generation->convention->foreignPropertyName(property->propertyName, className));
            foreign->metaType = property->metaType;
            tuneProperty(index, foreign, generation, false);
            generation->object->foreignProperty = MetaProperty(foreign);
        }
    }

    // Is label ?
    if (generation->object->labelPropertyIndex < 0) {
        if (property->propertyName == generation->info(META_LABEL))
            property->attributes.setFlag(MetaProperty::LabelProperty, true);

        if (property->attributes.testFlag(MetaProperty::LabelProperty))
            generation->object->labelPropertyIndex = index;
    }

    if (property->propertyName == generation->info(META_CREATED_AT, "createdAt")) {
        property->attributes.setFlag(MetaProperty::CreationTimestamp);
        property->attributes.setFlag(MetaProperty::FillableProperty, false);
        generation->object->creationTimestampIndex = index;
    }

    if (property->propertyName == generation->info(META_UPDATED_AT, "updatedAt")) {
        property->attributes.setFlag(MetaProperty::UpdateTimestamp);
        property->attributes.setFlag(MetaProperty::FillableProperty, false);
        generation->object->updateTimestampIndex = index;
    }

    if (property->propertyName == generation->info(META_DELETED_AT, "deletedAt")) {
        property->attributes.setFlag(MetaProperty::CreationTimestamp);
        property->attributes.setFlag(MetaProperty::FillableProperty, false);
        generation->object->deletionTimestampIndex = index;
    }

    // Registering property
    if (save)
        generation->object->properties.append(MetaProperty(property));
}

} // namespace QEloquent
