#include "user.h"

namespace Store {

UserRole::UserRole()
    : QEloquent::Model(this)
{}

User::User()
    : QEloquent::Model(this)
{}

QEloquent::Relation<UserRole> User::role() const
{
    return belongsTo<UserRole>("role_id");
}

} // namespace Store
