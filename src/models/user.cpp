#include "user.h"

namespace QEloquent {

User::User() {}

QString User::name() const
{
    return m_name;
}

void User::setName(const QString &name)
{
    if (m_name == name)
        return;
    m_name = name;
    emit nameChanged();
}

QString User::email() const
{
    return m_email;
}

void User::setEmail(const QString &email)
{
    if (m_email == email)
        return;
    m_email = email;
    emit emailChanged();
}

QString User::password() const
{
    return m_password;
}

void User::setPassword(const QString &password)
{
    if (m_password == password)
        return;
    m_password = password;
    emit passwordChanged();
}

QDateTime User::createdAt() const
{
    return m_createdAt;
}

void User::setCreatedAt(const QDateTime &createdAt)
{
    if (m_createdAt == createdAt)
        return;
    m_createdAt = createdAt;
    emit createdAtChanged();
}

QDateTime User::updatedAt() const
{
    return m_updatedAt;
}

void User::setUpdatedAt(const QDateTime &updatedAt)
{
    if (m_updatedAt == updatedAt)
        return;
    m_updatedAt = updatedAt;
    emit updatedAtChanged();
}

Relation<UserRole> User::roles() const
{
    return belongsToMany<UserRole>();
}

Relation<UserGroup> User::groups() const
{
    return belongsToMany<UserGroup>();
}

} // namespace QEloquent
