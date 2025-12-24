#include "product.h"

namespace Store {

Product::Product()
    : SmartModel(this)
{}

Relation<Stock> Product::stock() const
{
    return hasOne<Stock>();
}

Relation<Category> Product::category() const
{
    return belongsTo<Category>();
}

Category::Category()
    : SmartModel(this)
{}

Relation<Product> Category::products() const
{
    return hasMany<Product>();
}

Stock::Stock()
    : SmartModel(this)
{}

Relation<Product> Stock::product() const
{
    return belongsTo<Product>();
}

} // namespace Store
