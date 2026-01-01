#ifndef SIMPLEMODELSTEST_H
#define SIMPLEMODELSTEST_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

class SimpleProduct : public QEloquent::Model, public QEloquent::ModelHelpers<SimpleProduct>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(QString barcode MEMBER barcode)
    Q_PROPERTY(QString createdAt MEMBER createdAt)
    Q_PROPERTY(QString updatedAt MEMBER updatedAt)
    // categoryId: int (dynamic)

    Q_CLASSINFO("table", "Products")
    Q_CLASSINFO("hidden", "categoryId")

public:
    SimpleProduct() : QEloquent::Model(this) {}

    int id = 0;
    QString name;
    QString description;
    double price = 0.0;
    QString barcode;
    QString createdAt;
    QString updatedAt;
};

class SimpleCategory : public QEloquent::Model, public QEloquent::ModelHelpers<SimpleCategory>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)

public:
    SimpleCategory() : QEloquent::Model(this) {}

    int id = 0;
    QString name;
    QString description;
};

#endif // SIMPLEMODELSTEST_H
