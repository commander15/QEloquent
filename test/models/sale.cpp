#include "sale.h"
#include "user.h"
#include "product.h"
#include "usergroup.h"

using namespace QEloquent;

Sale::Sale()
    : QEloquent::Model(this)
{}

QEloquent::Relation<User> Sale::seller() const
{
    return belongsTo<User>("seller_id");
}

QEloquent::Relation<UserGroup> Sale::groups() const
{
    return belongsToManyThrough<UserGroup, User>("UserGroupMembers", "user_id", "group_id", "seller_id");
}

SaleItem::SaleItem()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Sale> SaleItem::sale() const
{
    return belongsTo<Sale>();
}

QEloquent::Relation<Product> SaleItem::product() const
{
    return belongsTo<Product>();
}
