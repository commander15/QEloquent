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

    Q_INVOKABLE QString since() const;

    int id = 0;
    QString name;
    QString description;
    double price = 0.0;
    QString barcode;
    QDateTime createdAt;
    QDateTime updatedAt;
};

class SimpleCategory : public QEloquent::Model, public QEloquent::ModelHelpers<SimpleCategory>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)

public:
    SimpleCategory();

    int id = 0;
    QString name;
    QString description;
};

#endif // SIMPLEMODELS_H
