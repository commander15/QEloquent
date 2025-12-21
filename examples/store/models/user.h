#ifndef STORE_USER_H
#define STORE_USER_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

namespace Store {

class UserRole : public QEloquent::Model, public QEloquent::ModelHelpers<UserRole>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)

    Q_CLASSINFO("table", "UserRoles")

public:
    UserRole();

    int id = 0;
    QString name;
};

class User : public QEloquent::Model, public QEloquent::ModelHelpers<User>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString password MEMBER password)
    Q_PROPERTY(int roleId MEMBER roleId)

    Q_CLASSINFO("table", "Users")
    Q_CLASSINFO("fillable", "name,email,password,role_id")
    Q_CLASSINFO("hidden", "password")

public:
    User();

    int id = 0;
    QString name;
    QString email;
    QString password;
    int roleId = 0;

    Q_INVOKABLE QEloquent::Relation<UserRole> role() const;
};

} // namespace Store

#endif // STORE_USER_H
