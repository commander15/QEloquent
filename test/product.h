#ifndef PRODUCT_H
#define PRODUCT_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

class Stock;

class Product : public QEloquent::Model, public QEloquent::ModelHelpers<Product>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(QString barcode MEMBER barcode)
    Q_PROPERTY(int categoryId MEMBER categoryId)
    Q_PROPERTY(QString createdAt MEMBER createdAt)
    Q_PROPERTY(QString updatedAt MEMBER updatedAt)

    Q_CLASSINFO("fillable", "name, description, price, barcode, categoryId")
    Q_CLASSINFO("hidden", "categoryId")
    Q_CLASSINFO("table", "Products")
    Q_CLASSINFO("append", "stock")

public:
    Product();

    Q_INVOKABLE Stock stock() const;

    int id = 0;
    QString name;
    QString description;
    double price = 0.0;
    QString barcode;
    int categoryId = 0;
    QString createdAt;
    QString updatedAt;
};

class Stock : public QEloquent::Model, public QEloquent::ModelHelpers<Stock>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(int productId MEMBER productId)
    Q_PROPERTY(QString createdAt MEMBER createdAt)
    Q_PROPERTY(QString updatedAt MEMBER updatedAt)

    Q_CLASSINFO("table", "Stocks")

public:
    Stock();

    int id = 0;
    int productId = 0;
    QString createdAt;
    QString updatedAt;
};

#endif // PRODUCT_H
