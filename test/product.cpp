#include "product.h"

Product::Product()
    : QEloquent::Model(this)
{}

Stock Product::stock() const
{
    QEloquent::Query query;
    query.where("product_id", id);

    auto result = Stock::find(query);
    if (result && !result->isEmpty())
        return result->first();
    else
        return Stock();
}

Stock::Stock()
    : QEloquent::Model(this)
{}
