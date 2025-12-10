#include "metarelation.h"

#include <QEloquent/model.h>
#include <QEloquent/private/model_p.h>

#include <QVariant>
#include <QMetaMethod>

class QMetaMethod;

namespace QEloquent {

class MetaRelationData : public QSharedData
{
public:
    QString relationName;
    QMetaMethod *getter = nullptr;
};

MetaRelation::MetaRelation()
    : data(new MetaRelationData)
{}

MetaRelation::MetaRelation(const MetaRelation &rhs)
    : data{rhs.data}
{}

MetaRelation::MetaRelation(MetaRelation &&rhs)
    : data{std::move(rhs.data)}
{}

MetaRelation &MetaRelation::operator=(const MetaRelation &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

MetaRelation &MetaRelation::operator=(MetaRelation &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

MetaRelation::~MetaRelation() {}

QString MetaRelation::relationName() const
{
    return data->relationName;
}

QVariant MetaRelation::read(Model *model) const
{
    if (data->getter != nullptr) {
        return data->getter->invokeOnGadget(model);
    }

    return model->data->dynamicProperties.value(data->relationName);
}

} // namespace QEloquent
