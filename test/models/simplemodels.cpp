#include "simplemodels.h"

SimpleProduct::SimpleProduct() : QEloquent::Model(this)
{}

QString SimpleProduct::since() const
{
    return "since " + QString::number(createdAt.date().year());
}

SimpleCategory::SimpleCategory() : QEloquent::Model(this)
{}
