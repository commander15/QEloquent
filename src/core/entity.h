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
    bool get();

    bool save();
    bool insert();
    bool update();

    bool deleteData();

    bool run(Operation op);

protected:
    virtual bool onGet();
    virtual bool doGet() = 0;
    virtual void afterGet();

    virtual bool onInsert();
    virtual bool doInsert() = 0;
    virtual void afterInsert();

    virtual bool onUpdate();
    virtual bool doUpdate() = 0;
    virtual void afterUpdate();

    virtual bool onDelete();
    virtual bool doDelete() = 0;
    virtual void afterDelete();
};

} // namespace QEloquent

#endif // QELOQUENT_ENTITY_H
