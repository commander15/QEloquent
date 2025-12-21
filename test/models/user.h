#ifndef USER_H
#define USER_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

class UserGroup;

class User : public QEloquent::Model, public QEloquent::ModelHelpers<User>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString password MEMBER password)

    Q_CLASSINFO("fillable", "name,email,password")
    Q_CLASSINFO("hidden", "password")

public:
    User();

    int id = 0;
    QString name;
    QString email;
    QString password;

    Q_INVOKABLE QEloquent::Relation<UserGroup> groups() const;
};

#endif // USER_H
