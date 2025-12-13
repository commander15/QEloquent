#ifndef QELOQUENT_ENTITY_H
#define QELOQUENT_ENTITY_H

#include <QEloquent/global.h>

namespace QEloquent {

class QELOQUENT_EXPORT Entity
{
public:
    enum Operation {
        GetOperation,
        InsertOperation,
        UpdateOperation,
        SaveOperation,
        DeleteOperation
    };

    virtual ~Entity() = default;

    virtual bool exists() = 0;
    virtual bool get() = 0;

    virtual bool save() { return (exists() ? update() : insert()); }
    virtual bool insert() = 0;
    virtual bool update() = 0;

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
