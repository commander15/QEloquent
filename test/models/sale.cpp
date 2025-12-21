#include "sale.h"
#include "user.h"
#include "product.h"

using namespace QEloquent;

Sale::Sale()
    : QEloquent::Model(this)
{}

QEloquent::Relation<User> Sale::seller() const
{
    return belongsTo<User>("sellerId");
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
