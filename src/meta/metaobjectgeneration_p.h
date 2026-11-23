#ifndef Q_ELOQUENT_METAOBJECTGENERATION_P_H
#define Q_ELOQUENT_METAOBJECTGENERATION_P_H

#include <QEloquent/metaobject.h>
#include <QEloquent/metaproperty.h>

#include <QMetaType>
#include <QList>
#include <QMap>

#define META_TABLE              "table"
#define META_PRIMARY            "primary"
#define META_LABEL              "label"
#define META_FOREIGN            "foreign"
#define META_CREATED_AT         "createdAt"
#define META_UPDATED_AT         "updatedAt"
#define META_DELETED_AT         "deletedAt"

#define META_DYNAMIC            "dynamic"
#define META_COMPUTED           "computed, append"
#define META_FILLABLE           "fillable"
#define META_HIDDEN             "hidden"
#define META_MAPPED             "map, mapped"
#define META_GENERATED          "generated"
#define META_WITH               "with"

#define META_PROPERTY_FIELD     "%1Field"

#define META_NAMING             "naming"
#define META_CONNECTION         "connection"

namespace QEloquent {

class MetaObjectGeneration
{
public:
    MetaObjectGeneration(const QMetaObject *qtMetaObject);

    MetaObject generate();
    QMap<MetaProperty::PropertyAttributeFlag, QStringList> propertyAttributes() const;

    bool hasInfo(const QString &name) const;
    QString info(const QString &name, const QString &defaultValue = QString()) const;
    QStringList infoList(const QString &name, const QString &defaultValue = QString()) const;
    QStringList rawInfo(const QString &name, int depth = 100) const;

private:
    struct PropertyOptions {
        QMetaType metaType;
        bool mappable = false;
        MetaProperty::PropertyAttributes attributes;
    };

    void discoverProperties();
    MetaPropertyData *addProperty(const QString &name, MetaProperty::PropertyType type, const PropertyOptions &options, bool save = true);

    MetaObjectPrivate *data;
    const QMetaObject *qtMetaObject;

    QMap<MetaProperty::PropertyAttributeFlag, QStringList> m_propertyAttributes;
    QList<MetaPropertyData *> m_properties;
    static const QString s_propertyAll;
};

class MetaObjectContext
{
public:
    MetaObjectContext(const QMetaObject *metaObject);

    QString primaryProperty() const;
    QString labelProperty() const;
    QStringList generatedProperties() const;
    QStringList fillableProperties() const;
    QStringList hiddenProperties() const;
    QStringList mappedProperties() const;
    QStringList typedProperties(MetaProperty::PropertyType type) const;

    struct Prop { QString name; MetaProperty::PropertyAttributes attribs; MetaProperty::PropertyType type; QMetaType metaType; };
    void foreachProperty(std::function<void(const Prop &prop)> callback);

    bool hasUserProperty() const;
    QString userPropertyName() const;

    bool hasProperty(const QString &name) const;
    QStringList propertyNames() const;

    bool hasMethod(const QString &name) const;
    QStringList methodNames() const;

    bool hasInfo(const QString &name) const;
    QString info(const QString &name, const QString &defaultValue = QString()) const;
    QStringList infoList(const QString &name, const QString &defaultValue = QString()) const;
    QStringList infoList(const QString &name, const QStringList &defaultValue) const;
    QStringList rawInfo(const QString &name) const;

private:
    const QMetaObject *m_metaObject;
    QMap<MetaProperty::PropertyType, QStringList> m_properties;
    QMap<QString, QMetaType> m_metaTypes;
};

} // namespace QEloquent

#endif // Q_ELOQUENT_METAOBJECTGENERATION_P_H
