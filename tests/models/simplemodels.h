#ifndef SIMPLEMODELS_H
#define SIMPLEMODELS_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

#include <QDateTime>

class SimpleProduct : public QEloquent::Model, public QEloquent::ModelHelpers<SimpleProduct>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(QString barcode MEMBER barcode)
    Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
    Q_PROPERTY(QDateTime updatedAt MEMBER updatedAt)
    // categoryId: int (dynamic)

    Q_CLASSINFO("table", "Products")
    Q_CLASSINFO("foreign", "productId")
    Q_CLASSINFO("hidden", "categoryId")
    Q_CLASSINFO("append", "since")

public:
    SimpleProduct();
    template<typename T> SimpleProduct(T *m) : QEloquent::Model(m) {}
    virtual ~SimpleProduct() = default;

    Q_INVOKABLE QString since() const;

    int id = 0;
    QString name;
    QString description;
    double price = 0.0;
    QString barcode;
    QDateTime createdAt;
    QDateTime updatedAt;
};

class SimpleStock : public QEloquent::Model, public QEloquent::ModelHelpers<SimpleStock>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(int quantity MEMBER quantity)
    Q_PROPERTY(QString createdAt MEMBER createdAt)
    Q_PROPERTY(QString updatedAt MEMBER updatedAt)

    Q_CLASSINFO("table", "Stocks")
    Q_CLASSINFO("hidden", "productId")

public:
    SimpleStock();
    template<typename T> SimpleStock(T *m) : QEloquent::Model(m) {}
    virtual ~SimpleStock() = default;

    int id = 0;
    int quantity = 0;
    QString createdAt;
    QString updatedAt;
};

class SimpleCategory : public QEloquent::Model, public QEloquent::ModelHelpers<SimpleCategory>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
    Q_PROPERTY(QDateTime updatedAt MEMBER updatedAt)

public:
    SimpleCategory();
    template<typename T> SimpleCategory(T *m) : QEloquent::Model(m) {}
    virtual ~SimpleCategory() = default;

    int id = 0;
    QString name;
    QString description;
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // SIMPLEMODELS_H
