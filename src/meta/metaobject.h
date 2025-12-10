#ifndef QELOQUENT_METAOBJECT_H
#define QELOQUENT_METAOBJECT_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

class QMetaObject;
class QMetaProperty;
class QMetaMethod;

class QSqlRecord;
class QSqlField;

namespace QEloquent {

class MetaProperty;
class MetaRelation;
class Model;
class Connection;

class MetaObjectPrivate;
class QELOQUENT_EXPORT MetaObject
{
public:
    enum PropertyResolution {
        ResolveByPropertyName,
        ResolveByFieldName
    };

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

    bool hasCreationTimestamp() const;
    MetaProperty creationTimestamp() const;

    bool hasUpdateTimestamp() const;
    MetaProperty updateTimestamp() const;

    bool hasDeletionTimestamp() const;
    MetaProperty deletionTimestamp() const;

    MetaProperty property(const QString &name, PropertyResolution resolution = ResolveByPropertyName) const;
    QList<MetaProperty> properties() const;

    QVariantMap readFillableFields(const Model *model) const;
    QVariantMap readAllFields(const Model *model, bool excludePrimary = false) const;

    QStringList hiddenFieldNames() const;
    QStringList fillableFieldNames() const;
    QStringList appendFieldNames() const;
    QStringList relations() const;

    QString connectionName() const;
    Connection connection() const;

    bool isValid() const;

    template<typename T>
    static MetaObject from() { return fromQtMetaObject(T::staticMetaObject); }
    static MetaObject fromQtMetaObject(const QMetaObject &metaObject);

private:
    MetaObject(MetaObjectPrivate *data);

    QExplicitlySharedDataPointer<MetaObjectPrivate> d;

    friend class MetaObjectBuilder;
};

}

#endif // QELOQUENT_METAOBJECT_H
