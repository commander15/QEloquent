#ifndef QELOQUENT_ENTITY_H
#define QELOQUENT_ENTITY_H

#include <QEloquent/global.h>

#include <QSharedData>

#define ENTITY_INFO_DATA(Class) Class##Data &data = *static_cast<Class##Data *>(EntityInfo::data.get());

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

    virtual ~Entity();

    virtual bool exists() const = 0;
    virtual bool get() = 0;

    virtual bool save();
    virtual bool insert() = 0;
    virtual bool update() = 0;

    virtual bool deleteData() = 0;

    bool run(Operation op);
};

class EntityInfoData;
class QELOQUENT_EXPORT EntityInfo
{
public:
    EntityInfo();
    EntityInfo(const EntityInfo &other);
    EntityInfo(EntityInfo &&other);
    EntityInfo &operator=(const EntityInfo &other);
    EntityInfo &operator=(EntityInfo &&other);
    ~EntityInfo();

    QString table() const;
    QString foreignKey() const;
    QStringList with() const;

protected:
    EntityInfo(EntityInfoData *data);
    EntityInfo(const QSharedDataPointer<EntityInfoData> &data);

    QSharedDataPointer<EntityInfoData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_ENTITY_H
