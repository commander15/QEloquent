#ifndef SIMPLECRUDTEST_H
#define SIMPLECRUDTEST_H

#include <test.h>

#include <QEloquent/modelinfobuilder.h>

#include <QStringList>

class SimpleCRUDTest : public Test
{
protected:
    void SetUp() override
    {
        Test::SetUp();
        RegisterProductModel(false);
    }

    void RegisterProductModel(bool autoLoad)
    {
        QEloquent::ModelInfoBuilder product;
        product.table("Products");
        if (!autoLoad) {
            product.primaryKey("id").foreignKey("product_id");
            product.fillable({ "name", "description", "price" });
            product.hidden("category_id");
        }
        connection.registerModel(product);
    }
};

#endif // SIMPLECRUDTEST_H
