#ifndef QELOQUENT_SMARTMODEL_H
#define QELOQUENT_SMARTMODEL_H

#include <QEloquent/global.h>
#include <QEloquent/model_a.h>
#include <QEloquent/id.h>

#include <type_traits>

namespace QEloquent {

class SmartModelData;
class QELOQUENT_EXPORT SmartModel : public Model
{
    Q_GADGET
    Q_PROPERTY(QEloquent::ID id READ id WRITE setId FINAL)
    Q_PROPERTY(QEloquent::Timestamp createdAt READ createdAt WRITE setCreatedAt FINAL)
    Q_PROPERTY(QEloquent::Timestamp updatedAt READ updatedAt WRITE setUpdatedAt FINAL)
    Q_PROPERTY(QEloquent::Timestamp deletedAt READ deletedAt WRITE setDeletedAt FINAL)

public:
    enum IDType {
        IntegerID = ID::IntegerID,
        StringID = ID::StringID,
        UuidID = ID::UuidID
    };

    SmartModel(const SmartModel &other);
    SmartModel(SmartModel &&other);
    SmartModel &operator=(const SmartModel &other);
    SmartModel &operator=(SmartModel &&other);
    virtual ~SmartModel();

    template<typename T> T id() const { return id().get<T>(); }
    QEloquent::ID id() const;
    bool setId(const QEloquent::ID &id);

    QEloquent::Timestamp createdAt() const;
    QEloquent::Timestamp updatedAt() const;
    QEloquent::Timestamp deletedAt() const;

protected:
    template<typename T, typename = std::enable_if_t<std::is_base_of_v<SmartModel, T>>>
    SmartModel(T *, IDType idType = IntegerID)
        : SmartModel(T::staticMetaObject, idType) {}

    SmartModel(const QMetaObject &metaObject, IDType idType);

private:
    void setCreatedAt(const QEloquent::Timestamp &time);
    void setUpdatedAt(const QEloquent::Timestamp &time);
    void setDeletedAt(const QEloquent::Timestamp &time);
};

} // namespace QEloquent

#endif // QELOQUENT_SMARTMODEL_H