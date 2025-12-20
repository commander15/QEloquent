#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

#include <QDateTime>

class TestModel : public QEloquent::Model, public QEloquent::ModelHelpers<TestModel>
{
    Q_GADGET

    // Fields
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(double price MEMBER price)
    Q_PROPERTY(int categoryId MEMBER categoryId)
    Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
    Q_PROPERTY(QDateTime updatedAt MEMBER updatedAt)
    Q_PROPERTY(QDateTime deletedAt MEMBER deletedAt)

    // To override table name generation from class name
    Q_CLASSINFO("table", "Products")

    // To override primary key
    Q_CLASSINFO("primary", "id")

    // To restrict filling to desired columns instead of all properties that are writable though Meta Object System
    Q_CLASSINFO("fillable", "name, description, price")

    // Properties to hide during serialization (to JSON for example)
    Q_CLASSINFO("hidden", "categoryId")

    // Releations to load when retrieving model, they must be registered as Q_INVOKABLE first
    // and must just return a predefined function call with some templating (hasOne, ...)
    Q_CLASSINFO("with", "subTests")

    // Naming convention to use to convert class/propery name to table/field name
    // the "Laravel" naming convention convert class/property names to Laravel compliant one
    // More naming convention will be added later
    Q_CLASSINFO("naming", "Laravel")

    // If you don't want to use the default connection
    Q_CLASSINFO("connection", "DB2")

public:
    TestModel();

    // Some raw function defined by user, not handled by QEloquent
    void someRawFunction()
    {}

    // A well defined relation
    Q_INVOKABLE QEloquent::Relation<TestModel> subTests()
    { return hasOne<TestModel>(); }

    int id = 0;

    QString name;
    QString description;
    double price = 0.0;
    int categoryId = 0;

    QDateTime createdAt;
    QDateTime updatedAt;
    QDateTime deletedAt;
};

#endif // TESTMODEL_H
