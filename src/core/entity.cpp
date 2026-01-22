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
 * \fn bool Entity::get
 * \brief Fetch/retrieve operation (SELECT).
 *
 * Used when loading existing record(s) from the database.
 * \sa Entity::GetOperation and Entity::run
 */

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
 * \fn bool Entity::insert
 * \brief Insert/create operation (INSERT).
 *
 * Used when creating new record(s) that do not yet exist in the database.
 * \sa Entity::InsertOperation and Entity::run
 */

/*!
 * \fn bool Entity::update
 * \brief Update/modify operation (UPDATE).
 *
 * Used when modifying existing record(s) that are already persisted.
 * \sa Entity::UpdateOperation and Entity::run
 */

/*!
 * \fn bool Entity::deleteData
 * \brief Delete operation (DELETE).
 *
 * Used to delete existing record(s) that are already persisted.
 * \sa Entity::DeleteOperation and Entity::run
 */

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

}
