#ifndef QLOQUENT_MODEL_P_H
#define QLOQUENT_MODEL_P_H

#include "model.h"

#include <QEloquent/modelquery.h>
#include <QEloquent/modelerror.h>
#include <QEloquent/modelinfo.h>

#include <QVariant>
#include <QMetaProperty>

namespace QEloquent {

class ModelData : public QSharedData
{
public:
    virtual ~ModelData() = default;

    virtual QStringList fieldNames() const = 0;
    virtual bool hasValue(const QString &field, const Model *model) const
    { return !readValue(field, model).isNull(); }
    virtual QVariant readValue(const QString &field, const Model *model) const = 0;
    virtual bool writeValue(const QString &field, const QVariant &value, Model *model) = 0;

    virtual ModelData *clone() const = 0;

    ModelInfo info;
    ModelQuery lastQuery;
    ModelError lastError;

protected:
    QString propertyName(const QString &fieldName) const
    {
        QString name;
        bool upper = false;

        for (const QChar &c : fieldName) {
            if (c == '_') {
                upper = true;
            } else if (upper) {
                name.append(c.toUpper());
                upper = false;
            } else {
                name.append(c);
            }
        }

        return name;
    }

    QString fieldName(const QString &propertyName) const
    {
        QString name;
        for (const QChar &c : propertyName) {
            if (c.isLower())
                name.append(c);
            else
                name.append('_' + c.toLower());
        }
        return name;
    }

    QVariantMap fieldsData(const QStringList &fields, const Model *model) const
    {
        QVariantMap data;
        for (const QString &field : fields) {
            const QVariant value = readValue(field, model);
            if (value.isValid())
                data.insert(field, value);
        }
        return data;
    }

    friend class Model;
};

class MetaModelData final : public ModelData
{
public:
    MetaModelData(const QMetaObject *meta)
        : metaObject(meta) {}

    QStringList fieldNames() const override
    {
        QStringList names;

        const QMetaObject *meta = &Model::staticMetaObject;
        for (int i(0); i < meta->propertyCount(); ++i)
            names.append(fieldName(meta->property(i).name()));

        return names;
    }

    QVariant readValue(const QString &field, const Model *model) const override
    {
        const QString name = propertyName(field);
        const int index = metaObject->indexOfProperty(name.toStdString().c_str());
        return (index < 0 ? QVariant() : metaObject->property(index).readOnGadget(model));
    }

    bool writeValue(const QString &field, const QVariant &value, Model *model) override
    {
        const QString name = propertyName(field);
        const int index = metaObject->indexOfProperty(name.toStdString().c_str());
        return (index < 0 ? false : metaObject->property(index).writeOnGadget(model, value));
    }

    MetaModelData *clone() const override
    {
        return new MetaModelData(*this);
    }

    const QMetaObject *metaObject;
};

} // namespace QELoquent

#endif // QLOQUENT_MODEL_P_H
