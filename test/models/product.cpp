#include "product.h"

Product::Product()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Stock> Product::stock() const
{
    return hasOne<Stock>("product_id");
}

QEloquent::Relation<Category> Product::category() const
{
    return belongsTo<Category>();
}

Stock::Stock()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Product> Stock::product() const
{
    return belongsTo<Product>("product_id");
}

Category::Category()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Product> Category::products() const
{
    return hasMany<Product>("category_id");
}
