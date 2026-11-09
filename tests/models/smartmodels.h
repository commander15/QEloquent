#ifndef SMARTMODELS_H
#define SMARTMODELS_H

#include <QEloquent/model.h>

class SmartProduct : public QEloquent::SmartModel, public QEloquent::ModelHelpers<SmartProduct>
{
    Q_GADGET
    Q_PROPERTY(QEloquent::Data<QString> name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(QString barcode MEMBER barcode)
    // categoryId: int (dynamic)

    Q_CLASSINFO("table", "Products")
    Q_CLASSINFO("foreign", "productId")
    Q_CLASSINFO("hidden", "categoryId")
    Q_CLASSINFO("append", "since")

public:
    SmartProduct() : QEloquent::SmartModel(this, IntegerID) { }

    QEloquent::Data<QString> name;
    QString description;
    double price = 0.0;
    QString barcode;
};

class SmartTestModel : public QEloquent::SmartModel
{
    Q_GADGET
    Q_PROPERTY(QEloquent::Timestamp testedAt MEMBER testedAt)
    Q_CLASSINFO("generated", "testedAt")

public:
    SmartTestModel() : QEloquent::SmartModel(this, UuidID) {}

    QEloquent::Timestamp testedAt;
};

#endif // SMARTMODELS_H
