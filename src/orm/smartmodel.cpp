#include "smartmodel.h"

#include "model_p.h"

#define MODEL_DATA(Class) Class##Data &data = *static_cast<Class##Data *>(Model::data.get());

namespace QEloquent {

class SmartModelData : public ModelData
{
public:
    ID id;
    QDateTime createdAt;
    QDateTime updatedAt;
    QDateTime deletedAt;
};

SmartModel::SmartModel(const QMetaObject &metaObject)
    : Model(metaObject, new SmartModelData())
{
    MODEL_DATA(SmartModel);
    data.id = ID::make(0);

    const MetaProperty prop = data.metaObject.property("id");
    if (prop.isValid()) {
        if (prop.metaType() == QMetaType::fromType<QString>())
            data.id = ID::make(QString());
    }

    data.id.unsetValue();
}

SmartModel::SmartModel(const SmartModel &other)
    : Model(other)
{}

SmartModel::SmartModel(SmartModel &&other)
    : Model(other)
{}

SmartModel::~SmartModel()
{}

ID SmartModel::id() const
{
    MODEL_DATA(const SmartModel);
    return data.id;
}

bool SmartModel::setId(ID id)
{
    MODEL_DATA(SmartModel);

    if (id.type() != data.id.type())
        return false;

    data.id = id;
    return true;
}

QDateTime SmartModel::createdAt() const
{
    MODEL_DATA(const SmartModel);
    return data.createdAt;
}

QDateTime SmartModel::updatedAt() const
{
    MODEL_DATA(const SmartModel);
    return data.updatedAt;
}

QDateTime SmartModel::deletedAt() const
{
    MODEL_DATA(const SmartModel);
    return data.deletedAt;
}

void SmartModel::setCreatedAt(const QDateTime &time)
{
    MODEL_DATA(SmartModel);
    data.createdAt = time;
}

void SmartModel::setUpdatedAt(const QDateTime &time)
{
    MODEL_DATA(SmartModel);
    data.updatedAt = time;
}

void SmartModel::setDeletedAt(const QDateTime &time)
{
    MODEL_DATA(SmartModel);
    data.deletedAt = time;
}

} // namespace QEloquent
