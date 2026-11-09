#include "entity.h"

namespace QEloquent {

/*!
 * \class Entity
 * \brief The base class for entities (Model or Relation).
 */

/*!
 * \enum Entity::Operation
 * \brief Represents the type of database operation to perform.
 *
 * This enumeration defines the standard CRUD-like operations supported by QEloquent.
 * It is used internally to determine the correct SQL generation, validation, and
 * behavior during model persistence.
 *
 * \var Entity::GetOperation
 * \brief Fetch/retrieve operation (SELECT).
 */

/*!
 * \var Entity::InsertOperation
 * \brief Insert/create operation (INSERT).
 */

/*!
 * \var Entity::UpdateOperation
 * \brief Update/modify operation (UPDATE).
 */

/*!
 * \var Entity::SaveOperation
 * \brief Save (upsert) operation.
 */

/*!
 * \var Entity::DeleteOperation
 * \brief Delete operation (DELETE).
 */

/*!
 * \fn bool Entity::exists
 * \brief Check if the entity exists on the Database.
 */

/*!
 * \brief Fetch/retrieve operation (SELECT).
 *
 * Used when loading existing record(s) from the database.
 * \sa Entity::GetOperation and Entity::run
 */
bool Entity::get()
{
    if (!onGet() || !doGet())
        return false;

    afterGet();
    return true;
}

/*!
 * \brief Persists the entity (executes insert or update).
 *
 * Combines insert and update behavior: inserts if the record does not exist,
 * updates if it does. Typically used for "save or update" semantics.
 * \sa Entity::SaveOperation and Entity::run
 */
bool Entity::save()
{
    return (exists() ? update() : insert());
}

/*!
 * \brief Insert/create operation (INSERT).
 *
 * Used when creating new record(s) that do not yet exist in the database.
 * \sa Entity::InsertOperation and Entity::run
 */
bool Entity::insert()
{
    if (!onInsert() || !doInsert())
        return false;

    afterInsert();
    return true;
}

/*!
 * \brief Update/modify operation (UPDATE).
 *
 * Used when modifying existing record(s) that are already persisted.
 * \sa Entity::UpdateOperation and Entity::run
 */
bool Entity::update()
{
    if (!onUpdate() || !doUpdate())
        return false;

    afterUpdate();
    return true;
}

/*!
 * \brief Delete operation (DELETE).
 *
 * Used to delete existing record(s) that are already persisted.
 * \sa Entity::DeleteOperation and Entity::run
 */
bool Entity::deleteData()
{
    if (!onDelete() || !doDelete())
        return false;

    afterDelete();
    return true;
}

/*!
 * \brief Run the operation op on the entity.
 * \return true if succeeded, false otherwise.
 */
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

bool Entity::onGet()
{
    return true;
}

void Entity::afterGet()
{
}

bool Entity::onInsert()
{
    return true;
}

void Entity::afterInsert()
{
}

bool Entity::onUpdate()
{
    return exists();
}

void Entity::afterUpdate()
{
}

bool Entity::onDelete()
{
    return exists();
}

void Entity::afterDelete()
{
}

}
