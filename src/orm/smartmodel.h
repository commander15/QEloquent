#ifndef SMARTMODEL_H
#define SMARTMODEL_H

#include <QEloquent/global.h>
#include <QEloquent/model.h>
#include <QEloquent/id.h>

namespace QEloquent {

class SmartModelData;
class QELOQUENT_EXPORT SmartModel : public Model
{
    Q_GADGET
    Q_PROPERTY(QEloquent::ID id READ id WRITE setId FINAL)
    Q_PROPERTY(QDateTime createdAt MEMBER m_createdAt FINAL)
    Q_PROPERTY(QDateTime updatedAt MEMBER m_updatedAt FINAL)
    Q_PROPERTY(QDateTime deletedAt MEMBER m_deletedAt FINAL)

public:
    SmartModel(const SmartModel &other);
    SmartModel(SmartModel &&other);
    SmartModel &operator=(const SmartModel &);
    SmartModel &operator=(SmartModel &&);
    virtual ~SmartModel();

    QEloquent::ID id() const;
    bool setId(QEloquent::ID id);

    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    QDateTime deletedAt() const;

protected:
    template<typename T, std::enable_if<std::is_base_of<SmartModel, T>::value>::type* = nullptr>
    SmartModel(T *m) : Model(m) {}

    SmartModel(const QMetaObject &metaObject);

private:
    void setCreatedAt(const QDateTime &time);
    void setUpdatedAt(const QDateTime &time);
    void setDeletedAt(const QDateTime &time);
};

} // namespace QEloquent

#endif // SMARTMODEL_H
