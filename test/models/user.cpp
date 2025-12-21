#include "user.h"
#include "usergroup.h"

using namespace QEloquent;

User::User()
    : QEloquent::Model(this)
{}

QEloquent::Relation<UserGroup> User::groups() const
{
    return belongsToMany<UserGroup>("UserGroupMembers", "user_id", "group_id");
}
