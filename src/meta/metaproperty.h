#ifndef QELOQUENT_METAPROPERTY_H
#define QELOQUENT_METAPROPERTY_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>

class QMetaProperty;
class QMetaMethod;

namespace QEloquent {

class Model;

class MetaPropertyData;
class QELOQUENT_EXPORT MetaProperty
{
public:
    enum PropertyAttributeFlag {
        PrimaryProperty = 0x1,
        CreationTimestamp = 0x20,
        UpdateTimestamp = 0x40,
        DeletionTimestamp = 0x80,
        HiddenProperty = 0x1000,
        FillableProperty = 0x2000,
    };
    Q_DECLARE_FLAGS(PropertyAttributes, PropertyAttributeFlag)

    enum PropertyType {
        StandardProperty = 0x0,
        AppendedProperty = 0x10,
        RelationProperty = 0x80,
    };

    MetaProperty();
    MetaProperty(const MetaProperty &);
    MetaProperty(MetaProperty &&);
    MetaProperty &operator=(const MetaProperty &);
    MetaProperty &operator=(MetaProperty &&);
    ~MetaProperty();

    QString propertyName() const;
    QString fieldName() const;
    QMetaType metaType() const;

    bool hasAttribute(PropertyAttributeFlag flag) const;
    PropertyAttributes attributes() const;

    PropertyType propertyType() const;

    QVariant read(const Model *model) const;
    bool write(Model *model, const QVariant &value) const;

    bool isWritable() const;
    bool isValid() const;

    bool operator==(const MetaProperty &other) const;

private:
    MetaProperty(MetaPropertyData *data);

    QExplicitlySharedDataPointer<MetaPropertyData> data;

    friend class MetaObject;
    friend class MetaObjectGenerator;
};

} // namespace QEloquent

Q_DECLARE_OPERATORS_FOR_FLAGS(QEloquent::MetaProperty::PropertyAttributes)

#endif // QELOQUENT_METAPROPERTY_H
