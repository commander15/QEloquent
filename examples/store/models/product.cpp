#include "product.h"

namespace Store {

Product::Product()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Stock> Product::stock() const
{
    return hasOne<Stock>();
}

QEloquent::Relation<Category> Product::category() const
{
    return belongsTo<Category>();
}

Category::Category()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Product> Category::products() const
{
    return hasMany<Product>();
}

Stock::Stock()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Product> Stock::product() const
{
    return belongsTo<Product>();
}

} // namespace Store
