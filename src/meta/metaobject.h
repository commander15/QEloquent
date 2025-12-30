#ifndef QELOQUENT_METAOBJECT_H
#define QELOQUENT_METAOBJECT_H

#include <QEloquent/global.h>
#include <QEloquent/metaproperty.h>

#include <QSharedDataPointer>

class QMetaObject;
class QMetaProperty;
class QMetaMethod;

class QSqlRecord;
class QSqlField;

namespace QEloquent {

class MetaProperty;
class DataMap;
class Model;
class NamingConvention;
class Connection;

class MetaObjectPrivate;
class QELOQUENT_EXPORT MetaObject
{
public:
    enum PropertyNameResolution {
        ResolveByPropertyName,
        ResolveByFieldName
    };

    enum PropertyFilterFlag {
        StandardProperties = 0x1,
        AppendedProperties = 0x2,
        RelationProperties = 0x4,
        AllProperties = StandardProperties | AppendedProperties | RelationProperties
    };
    Q_DECLARE_FLAGS(PropertyFilters, PropertyFilterFlag)

    MetaObject();
    MetaObject(const MetaObject &);
    MetaObject(MetaObject &&);
    MetaObject &operator=(const MetaObject &);
    MetaObject &operator=(MetaObject &&);
    ~MetaObject();

    QString className() const;
    QString tableName() const;

    MetaProperty primaryProperty() const;
    MetaProperty foreignProperty() const;

    bool hasLabelProperty() const;
    MetaProperty labelProperty() const;

    bool hasCreationTimestamp() const;
    MetaProperty creationTimestamp() const;

    bool hasUpdateTimestamp() const;
    MetaProperty updateTimestamp() const;

    bool hasDeletionTimestamp() const;
    MetaProperty deletionTimestamp() const;

    MetaProperty property(const QString &name, PropertyNameResolution resolution = ResolveByPropertyName) const;
    QList<MetaProperty> properties(MetaProperty::PropertyAttributes attributes,
                                   PropertyFilters filters = AllProperties) const;
    QList<MetaProperty> properties() const;

    DataMap readProperties(const Model *model,
                               MetaProperty::PropertyAttributes attributes,
                               PropertyFilters filters = AllProperties,
                               PropertyNameResolution resolution = ResolveByPropertyName) const;
    int writeProperties(Model *model, const DataMap &data, PropertyNameResolution resolution = ResolveByPropertyName) const;

    DataMap readFillableFields(const Model *model) const;
    bool writeFillableFields(Model *model, const DataMap &data);

    QStringList hiddenFieldNames() const;
    QStringList fillableFieldNames() const;
    QStringList appendFieldNames() const;
    QStringList relations() const;

    NamingConvention *namingConvention() const;
    QString nammingConventionName() const;

    QString connectionName() const;
    Connection connection() const;

    bool isValid() const;

    template<typename T>
    static MetaObject from() { return fromQtMetaObject(T::staticMetaObject); }
    static MetaObject fromQtMetaObject(const QMetaObject &metaObject);

private:
    MetaObject(MetaObjectPrivate *data);

    QExplicitlySharedDataPointer<MetaObjectPrivate> d;

    friend class MetaObjectGenerator;
};

}

#endif // QELOQUENT_METAOBJECT_H
