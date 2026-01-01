#ifndef COMPLEXMODELS_H
#define COMPLEXMODELS_H

#include "simplemodels.h"
#include <QEloquent/relation.h>

class Stock;
class Category;

class Product : public SimpleProduct, public QEloquent::ModelHelpers<Product>
{
    Q_GADGET
    Q_CLASSINFO("with", "stock")
    QELOQUENT_HELPERS(Product)

public:
    Product();

    Q_INVOKABLE QEloquent::Relation<Stock> stock() const;
    Q_INVOKABLE QEloquent::Relation<Category> category() const;
};

class Stock : public SimpleStock, public QEloquent::ModelHelpers<Stock>
{
    Q_GADGET
    QELOQUENT_HELPERS(Stock)

public:
    Stock();

    Q_INVOKABLE QEloquent::Relation<Product> product() const;
};

class Category : public SimpleCategory, public QEloquent::ModelHelpers<Category>
{
    Q_GADGET
    QELOQUENT_HELPERS(Category)

public:
    Category();

    Q_INVOKABLE QEloquent::Relation<Product> products() const;
};

#endif // COMPLEXMODELS_H
