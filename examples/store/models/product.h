#ifndef STORE_PRODUCT_H
#define STORE_PRODUCT_H

#include "model.h"

namespace Store {

class Stock;
class Category;

class Product : public SmartModel, public ModelHelpers<Product>
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(QString barcode MEMBER barcode)
    Q_PROPERTY(int categoryId MEMBER categoryId)

    Q_CLASSINFO("table", "Products")

public:
    Product();

    QString name;
    QString description;
    double price = 0.0;
    QString barcode;
    int categoryId = 0;

    Q_INVOKABLE Relation<Stock> stock() const;
    Q_INVOKABLE Relation<Category> category() const;
};

class Category : public SmartModel, public ModelHelpers<Category>
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)

    Q_CLASSINFO("table", "Categories")

public:
    Category();

    QString name;
    QString description;

    Q_INVOKABLE Relation<Product> products() const;
};

class Stock : public SmartModel, public ModelHelpers<Stock>
{
    Q_GADGET
    Q_PROPERTY(int quantity MEMBER quantity)
    Q_PROPERTY(int productId MEMBER productId)

    Q_CLASSINFO("table", "Stocks")

public:
    Stock();

    int quantity = 0;
    int productId = 0;

    Q_INVOKABLE Relation<Product> product() const;
};

} // namespace Store

#endif // STORE_PRODUCT_H
