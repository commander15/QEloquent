#include "metaobjectbuilder.h"
#include "metatablebuilder_p.h"

#include <QEloquent/metaproperty.h>

namespace QEloquent {

MetaObjectBuilder::MetaObjectBuilder(const QString &className)
    : d(new MetaObjectBuilderPrivate(className))
{}

MetaObjectBuilder::~MetaObjectBuilder()
{}

MetaObjectBuilder &MetaObjectBuilder::table(const QString &name)
{
    d->tableName = name;
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::primary(const QString &property)
{
    d->primaryProperty = property;
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::foreign(const QString &property)
{
    d->foreignProperty = property;
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::hidden(const QString &property)
{
    d->hiddenProperties.append(property);
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::hidden(const QStringList &properties)
{
    d->hiddenProperties.append(properties);
    d->hiddenProperties.removeDuplicates();
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::fillable(const QString &property)
{
    d->fillableProperties.append(property);
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::fillable(const QStringList &properties)
{
    d->fillableProperties.append(properties);
    d->fillableProperties.removeDuplicates();
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::timestamps(const QString &creationProperty, const QString &updateProperty)
{
    d->creationTimestamp = creationProperty;
    d->updatedTimestamp = updateProperty;
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::timestamps(const QString &creationProperty, const QString &updateProperty, const QString &deletionProperty)
{
    d->deletionTimestamp = deletionProperty;
    return timestamps(creationProperty, updateProperty);
}

MetaObjectBuilder &MetaObjectBuilder::property(const QString &name, const QMetaType &type)
{
    return property(name, QString(), type);
}

MetaObjectBuilder &MetaObjectBuilder::property(const QString &name, const QString &field, const QMetaType &type)
{
    MetaPropertyData *property = new MetaPropertyData();
    property->propertyName = name;
    property->fieldName = field;
    property->type = type;
    d->properties.append(MetaProperty(property));
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::with(const QString &relation)
{
    d->relations.append(relation);
    d->relations.removeDuplicates();
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::with(const QStringList &relations)
{
    d->relations.append(relations);
    d->relations.removeDuplicates();
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::naming(const QString &name)
{
    d->convention = NamingConvention::convention(name);
    return *this;
}

MetaObjectBuilder &MetaObjectBuilder::connection(const QString &name)
{
    d->connectionName = name;
    d->connection = Connection::connection(name);
    return *this;
}

MetaObject MetaObjectBuilder::build()
{
    MetaObjectBuilderPrivate *d = MetaObjectBuilder::d.get();

    // We set properties field names and add them to sql record
    for (MetaProperty &property : d->properties) {
        if (property.fieldName.isEmpty())
            property.fieldName = d->convention->fieldName(property.propertyName, d->tableName);

        QSqlField field(property.fieldName, property.type);
        field.setDefaultValue(property.defaultValue);
        d->sqlRecord.append(field);
    }

    // If no primary key had been found, we use 'id'
    if (d->primaryProperty.isEmpty()) {
        d->primaryProperty = "id";

        if (!d->hasProperty(d->primaryProperty)) {
            MetaProperty primary;
            primary.propertyName = d->primaryProperty;
            primary.fieldName = d->convention->fieldName(d->primaryProperty, d->tableName);
            primary.type = QMetaType::fromType<int>();
            d->properties.prepend(primary);
        }
    }

    // If no foreign property provided, we guess it
    if (d->foreignProperty.isEmpty()) {
        const MetaProperty primary = d->property(d->primaryProperty);
        const QString foreignField = d->convention->foreignFieldName(primary.fieldName, d->tableName);
        d->foreignProperty = d->convention->foreignPropertyName(d.pr);
    }

    // If there is not valid connection, we issue a warning
    if (!d->connection.isValid()) {
        qWarning().noquote().nospace() << "QEloquent::MetaTableBuilder: no valid connection found for "
                                       << d->tableName << " meta table generation";
    }

    return MetaObject(d->build());
}

} // namespace QEloquent
