#include "product.h"

using namespace QEloquent;

Product::Product()
    : QEloquent::Model(this)
{
}

QString Product::fullDescription() const
{
    return name + " - " + description + " - at " + QString::number(price) + " XAF";
}

QString Product::priced() const
{
    return QString::number(price) + " XAF";
}

QEloquent::Relation<Stock> Product::stock() const
{
    return hasOne<Stock>();
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
    return belongsTo<Product>();
}

Category::Category()
    : QEloquent::Model(this)
{}

QEloquent::Relation<Product> Category::products() const
{
    return hasMany<Product>();
}

int Category::productCount() const
{
    auto result = Product::count(Query().where("category_id", id));
    return (result ? result.value() : 0);
}
