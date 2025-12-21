#include "entity.h"

namespace QEloquent {

/*!
 * \brief Persists the entity (executes insert or update).
 */
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

}
