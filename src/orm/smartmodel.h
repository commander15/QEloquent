#ifndef SMARTMODEL_H
#define SMARTMODEL_H

#include <QEloquent/global.h>
#include <QEloquent/model.h>
#include <QEloquent/id.h>

#include <type_traits>

namespace QEloquent {

class SmartModelData;
class QELOQUENT_EXPORT SmartModel : public Model
{
    Q_GADGET
    Q_PROPERTY(QEloquent::ID id READ id WRITE setId FINAL)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt FINAL)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt FINAL)
    Q_PROPERTY(QDateTime deletedAt READ deletedAt WRITE setDeletedAt FINAL)

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

    template<typename T> T id() const { return id().value().value<T>(); }
    QEloquent::ID id() const;
    bool setId(const QEloquent::ID &id);

    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    QDateTime deletedAt() const;

    bool insert() override;

protected:
    template<typename T, typename = std::enable_if_t<std::is_base_of_v<SmartModel, T>>>
    SmartModel(T *, IDType idType = IntegerID)
        : SmartModel(T::staticMetaObject, idType) {}

    SmartModel(const QMetaObject &metaObject, IDType idType);

private:
    void setCreatedAt(const QDateTime &time);
    void setUpdatedAt(const QDateTime &time);
    void setDeletedAt(const QDateTime &time);
};

} // namespace QEloquent

#endif // SMARTMODEL_H