#ifndef QELOQUENT_ENTITY_H
#define QELOQUENT_ENTITY_H

#include <QEloquent/global.h>
#include <QSharedDataPointer>

namespace QEloquent {

/**
 * @brief Base class for internal entity metadata.
 */
class QELOQUENT_EXPORT EntityInfo
{
public:
    /** @brief Virtual destructor */
    virtual ~EntityInfo() = default;
protected:
    /** @brief Protected constructor */
    EntityInfo() = default;
    /** @brief Internal data pointer */
    QSharedDataPointer<EntityInfoData> data;
};

/**
 * @brief Base class for any persistable object in QEloquent.
 */
class QELOQUENT_EXPORT Entity
{
public:
    /** @brief Operation types for binary commands */
    enum Operation {
        GetOperation,    /**< @brief Fetch data from DB */
        InsertOperation, /**< @brief Insert new record */
        UpdateOperation, /**< @brief Update existing record */
        SaveOperation,   /**< @brief Transparent insert/update */
        DeleteOperation  /**< @brief Delete record */
    };

    /** @brief Virtual destructor */
    virtual ~Entity() = default;

    /** @brief Returns true if the entity exists in the database */
    virtual bool exists() = 0;
    /** @brief Fetches the entity data from the database */
    virtual bool get() = 0;

    /** @brief Persists the entity (executes insert or update) */
    virtual bool save() { return (exists() ? update() : insert()); }
    /** @brief Inserts the entity as a new record */
    virtual bool insert() = 0;
    /** @brief Updates the existing record in the database */
    virtual bool update() = 0;

    /** @brief Deletes the entity from the database */
    virtual bool deleteData() = 0;

    bool run(Operation op)
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
};

} // namespace QEloquent

#endif // QELOQUENT_ENTITY_H
