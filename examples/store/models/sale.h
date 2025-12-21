#ifndef STORE_SALE_H
#define STORE_SALE_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>
#include "user.h"
#include "product.h"

namespace Store {

class SaleItem;

class Sale : public QEloquent::Model, public QEloquent::ModelHelpers<Sale>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(int number MEMBER number)
    Q_PROPERTY(double amount MEMBER amount)
    Q_PROPERTY(int sellerId MEMBER sellerId)

    Q_CLASSINFO("table", "Sales")

public:
    Sale();

    int id = 0;
    int number = 0;
    double amount = 0.0;
    int sellerId = 0;

    Q_INVOKABLE QEloquent::Relation<User> seller() const;
    Q_INVOKABLE QEloquent::Relation<SaleItem> items() const;
};

class SaleItem : public QEloquent::Model, public QEloquent::ModelHelpers<SaleItem>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(double unitPrice MEMBER unitPrice)
    Q_PROPERTY(int quantity MEMBER quantity)
    Q_PROPERTY(int saleId MEMBER saleId)
    Q_PROPERTY(int productId MEMBER productId)

    Q_CLASSINFO("table", "SaleItems")

public:
    SaleItem();

    int id = 0;
    double unitPrice = 0.0;
    int quantity = 0;
    int saleId = 0;
    int productId = 0;

    Q_INVOKABLE QEloquent::Relation<Sale> sale() const;
    Q_INVOKABLE QEloquent::Relation<Product> product() const;
};

} // namespace Store

#endif // STORE_SALE_H
