#include "entity.h"
#include "entity_p.h"

namespace QEloquent {

Entity::~Entity()
{
}

bool Entity::save()
{
    return (exists() ? update() : insert());
}

bool Entity::run(Operation op)
{
    switch (op) {
    case GetOperation:
        return get();

    case InsertOperation:
        return insert();

    case UpdateOperation:
        return update();

    case SaveOperation:
        return save();

    case DeleteOperation:
        return deleteData();
    }

    return false;
}

EntityInfo::EntityInfo()
    : data(new EntityInfoData())
{}

EntityInfo::EntityInfo(EntityInfoData *data)
    : data(data)
{}

EntityInfo::EntityInfo(const QSharedDataPointer<EntityInfoData> &data)
    : data(data)
{}

EntityInfo::EntityInfo(const EntityInfo &other)
    : data(other.data)
{}

EntityInfo::EntityInfo(EntityInfo &&other)
    : data(std::move(other.data))
{}

EntityInfo &EntityInfo::operator=(const EntityInfo &other)
{
    if (this != &other || data != other.data)
        data = other.data;
    return *this;
}

EntityInfo &EntityInfo::operator=(EntityInfo &&other)
{
    if (this != &other || data != other.data)
        data.swap(other.data);
    return *this;
}

EntityInfo::~EntityInfo()
{}

QString EntityInfo::table() const
{
    return data->table;
}

QString EntityInfo::foreignKey() const
{
    return data->foreignKey;
}

QStringList EntityInfo::with() const
{
    return data->with;
}

} // namespace QEloquent

template<>
QEloquent::EntityInfoData *QSharedDataPointer<QEloquent::EntityInfoData>::clone()
{ return d->clone(); }
