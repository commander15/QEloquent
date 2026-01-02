#ifndef COMPLEXMODELS_H
#define COMPLEXMODELS_H

#include "simplemodels.h"
#include <QEloquent/relation.h>

class Stock;
class Category;
class SaleItem;
class UserGroup;

class Product : public SimpleProduct, public QEloquent::ModelHelpers<Product>
{
    Q_GADGET
    Q_CLASSINFO("with", "stock")
    Q_CLASSINFO("append", "fullDescription, priced")
    QELOQUENT_HELPERS(Product)

public:
    Product();

    Q_INVOKABLE QString fullDescription() const;
    Q_INVOKABLE QString priced() const;

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
    Q_CLASSINFO("append", "productCount")
    QELOQUENT_HELPERS(Category)

public:
    Category();

    Q_INVOKABLE int productCount() const;
    Q_INVOKABLE QEloquent::Relation<Product> products() const;
    Q_INVOKABLE QEloquent::Relation<SaleItem> saleItems() const;
};

class User : public QEloquent::Model, public QEloquent::ModelHelpers<User>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString password MEMBER password)

    Q_CLASSINFO("table", "Users")
    Q_CLASSINFO("hidden", "password")

public:
    User();

    int id = 0;
    QString name;
    QString email;
    QString password;

    Q_INVOKABLE QEloquent::Relation<UserGroup> groups() const;
};

class UserGroup : public QEloquent::Model, public QEloquent::ModelHelpers<UserGroup>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)

    Q_CLASSINFO("table", "UserGroups")

public:
    UserGroup();

    int id = 0;
    QString name;

    Q_INVOKABLE QEloquent::Relation<User> users() const;
};

class Sale : public QEloquent::Model, public QEloquent::ModelHelpers<Sale>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(int number MEMBER number)
    Q_PROPERTY(double amount MEMBER amount)

    Q_CLASSINFO("hidden", "sellerId");
    Q_CLASSINFO("table", "Sales")

public:
    Sale();

    int id = 0;
    int number = 0;
    double amount = 0.0;

    Q_INVOKABLE QEloquent::Relation<User> seller() const;
    Q_INVOKABLE QEloquent::Relation<UserGroup> groups() const;
};

class SaleItem : public QEloquent::Model, public QEloquent::ModelHelpers<SaleItem>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(double unitPrice MEMBER unitPrice)
    Q_PROPERTY(int quantity MEMBER quantity)

    Q_CLASSINFO("hidden", "saleId, productId");
    Q_CLASSINFO("table", "SaleItems")

public:
    SaleItem();

    int id = 0;
    double unitPrice = 0.0;
    int quantity = 0;

    Q_INVOKABLE QEloquent::Relation<Sale> sale() const;
    Q_INVOKABLE QEloquent::Relation<Product> product() const;
};

#endif // COMPLEXMODELS_H
