#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

#include <QDateTime>

class TestModel : public QEloquent::Model, public QEloquent::ModelHelpers<TestModel>
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
    Q_PROPERTY(QDateTime updatedAt MEMBER updatedAt)
    Q_PROPERTY(QDateTime deletedAt MEMBER deletedAt)

    Q_CLASSINFO("table", "Products")
    Q_CLASSINFO("fillable", "name, description, price")
    Q_CLASSINFO("hidden", "category_id")

public:
    TestModel();

    QList<TestModel> subTests()
    { return hasOne<TestModel>(); }

    int id = 0;

    QString name;
    QString description;
    double price = 0.0;

    QDateTime createdAt;
    QDateTime updatedAt;
    QDateTime deletedAt;
};

#endif // TESTMODEL_H
