#include "simplemodels.h"

SimpleProduct::SimpleProduct() : QEloquent::Model(this)
{}

QString SimpleProduct::since() const
{
    return "since " + QString::number(createdAt.date().year());
}

SimpleStock::SimpleStock()
    : QEloquent::Model(this)
{}

SimpleCategory::SimpleCategory() : QEloquent::Model(this)
{}
