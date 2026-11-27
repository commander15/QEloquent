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

#define META_OPERATOR_INCLUSIVE "+"
#define META_OPERATOR_EXCLUSIVE "-"

namespace QEloquent {

class MetaObjectGeneration
{
public:
    using RawInfo = QPair<QString, QStringList>;
    using RawInfoList = QList<RawInfo>;
    MetaObjectGeneration(const QMetaObject *qtMetaObject);

    MetaObject generate();
    QMap<MetaProperty::PropertyType, QStringList> propertyTypes() const;
    QStringList propertyNames() const;
    QMap<MetaProperty::PropertyAttributeFlag, QStringList> propertyAttributes(const QStringList &properties) const;

    bool hasInfo(const QString &name) const;
    QString info(const QString &name, const QString &defaultValue = QString()) const;
    QStringList infoList(const QString &name, const QString &defaultValue) const;
    QStringList infoList(const QString &name, const QStringList &defaultValue = QStringList()) const;
    RawInfoList rawInfo(const QString &name, const QStringList suffixes = QStringList(), int depth = 100) const;
    QList<const QMetaObject *> getMetaObjects(bool fromBaseToChild) const;

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

    QList<MetaPropertyData *> m_properties;
    QMap<MetaProperty::PropertyAttributeFlag, QStringList> m_propertyAttributes;
    QList<std::function<void()>> m_pendingTasks;
    static const QString s_propertyAll;
};

} // namespace QEloquent

#endif // Q_ELOQUENT_METAOBJECTGENERATION_P_H
