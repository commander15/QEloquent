#include "complexmodels.h"

Product::Product()
    : SimpleProduct(this)
{}

QEloquent::Relation<Stock> Product::stock() const {
    return hasOne<Stock>();
}

QEloquent::Relation<Category> Product::category() const {
    return belongsTo<Category>();
}

Stock::Stock() :
    SimpleStock(this)
{}

QEloquent::Relation<Product> Stock::product() const {
    return belongsTo<Product>();
}

Category::Category()
    : SimpleCategory(this)
{}

QEloquent::Relation<Product> Category::products() const {
    return hasMany<Product>();
}
