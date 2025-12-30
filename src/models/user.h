#ifndef QELOQUENT_USER_H
#define QELOQUENT_USER_H

#include <QEloquent/model.h>

#include <QDateTime>

namespace QEloquent {

class UserRole;
class UserGroup;

class QELOQUENT_EXPORT User : public SimpleModel
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName FINAL)
    Q_PROPERTY(QString email READ email WRITE setEmail FINAL)
    Q_PROPERTY(QString password READ password WRITE setPassword FINAL)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt FINAL)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt FINAL)

public:
    template<typename T> User(T *m) : SimpleModel(m) {}
    virtual ~User() = default;

    QString name() const;
    void setName(const QString &name);

    QString email() const;
    void setEmail(const QString &email);

    QString password() const;
    void setPassword(const QString &password);

    QDateTime createdAt() const;
    void setCreatedAt(const QDateTime &createdAt);

    QDateTime updatedAt() const;
    void setUpdatedAt(const QDateTime &updatedAt);

    Relation<UserRole> roles() const;
    Relation<UserGroup> groups() const;

private:
    QString m_name;
    QString m_email;
    QString m_password;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

class QELOQUENT_EXPORT UserRole : public SimpleModel
{
    Q_GADGET

public:
    template<typename T> UserRole(T *m) : SimpleModel(m) {}

private:
    QString m_name;
    QString m_description;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

class QELOQUENT_EXPORT UserGroup : public UserRole {};

} // namespace QEloquent

#endif // QELOQUENT_USER_H
