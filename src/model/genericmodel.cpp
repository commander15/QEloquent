#include "genericmodel.h"
#include "genericmodel_p.h"

#include <QEloquent/private/model_p.h>

namespace QEloquent {

GenericModel::GenericModel()
    : GenericModel(ModelInfo())
{}

GenericModel::GenericModel(const ModelInfo &info)
    : Model(new GenericModelData(info))
{
}

GenericModel::GenericModel(const GenericModel &other)
    : Model(other)
{}

GenericModel::GenericModel(GenericModel &&other)
    : Model(std::move(other))
{}

GenericModel &GenericModel::operator=(const GenericModel &other)
{
    Model::operator=(other);
    return *this;
}

GenericModel &GenericModel::operator=(GenericModel &&other)
{
    Model::operator=(std::move(other));
    return *this;
}

GenericModel::~GenericModel()
{}

} // namespace QEloquent
