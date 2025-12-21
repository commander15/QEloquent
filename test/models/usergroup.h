#ifndef USERGROUP_H
#define USERGROUP_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

class User;

class UserGroup : public QEloquent::Model, public QEloquent::ModelHelpers<UserGroup>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)

    Q_CLASSINFO("table", "UserGroups")

public:
    UserGroup();

    int id = 0;
    QString name;

    Q_INVOKABLE QEloquent::Relation<User> users() const;
};

#endif // USERGROUP_H
