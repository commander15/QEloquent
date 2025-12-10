#ifndef QELOQUENT_GENERICMODEL_P_H
#define QELOQUENT_GENERICMODEL_P_H

#include <QEloquent/private/model_p.h>

namespace QEloquent {

class GenericModelData final : public ModelData
{
public:
    GenericModelData(const ModelInfo &info)
    { this->metaObject = info; }

    QStringList fieldNames() const override
    { return values.keys(); }

    bool hasValue(const QString &field, const Model *) const override
    { return values.contains(field); }

    QVariant readValue(const QString &field, const Model *) const override
    { return values.value(field); }

    bool writeValue(const QString &field, const QVariant &value, Model *) override
    { values.insert(field, value); return true; }

    GenericModelData *clone() const override
    { return new GenericModelData(*this); }

    QVariantMap values;
};

}

#endif // QELOQUENT_GENERICMODEL_P_H
