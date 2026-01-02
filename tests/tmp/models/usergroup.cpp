#include "usergroup.h"
#include "user.h"

using namespace QEloquent;

UserGroup::UserGroup()
    : QEloquent::Model(this)
{}

QEloquent::Relation<User> UserGroup::users() const
{
    return belongsToMany<User>("UserGroupMembers", "group_id", "user_id");
}
