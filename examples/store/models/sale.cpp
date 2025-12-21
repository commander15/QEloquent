#include "sale.h"

namespace Store {

Sale::Sale()
    : QEloquent::Model(this)
{}

QEloquent::Relation<User> Sale::seller() const
{
    return belongsTo<User>("seller_id");
}

QEloquent::Relation<SaleItem> Sale::items() const
{
    return hasMany<SaleItem>();
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

} // namespace Store
