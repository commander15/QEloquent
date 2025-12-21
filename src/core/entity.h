#ifndef QELOQUENT_ENTITY_H
#define QELOQUENT_ENTITY_H

#include <QEloquent/global.h>
#include <QSharedDataPointer>

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

    virtual bool save();
    virtual bool insert() = 0;
    virtual bool update() = 0;

    virtual bool deleteData() = 0;

    bool run(Operation op);
};

} // namespace QEloquent

#endif // QELOQUENT_ENTITY_H
