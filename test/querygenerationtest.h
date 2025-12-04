#ifndef QUERYGENERATIONTEST_H
#define QUERYGENERATIONTEST_H

#include "test.h"

#include <QEloquent/modelinfobuilder.h>

class QueryGenerationTest : public Test
{
protected:
    void SetUp() override
    {
        Test::SetUp();
        ASSERT_TRUE(migrate());

        QEloquent::ModelInfoBuilder product;
        product.table("Products");
        productModel = connection.registerModel(product);
    }

    QEloquent::ModelInfo productModel;
};

#endif // QUERYGENERATIONTEST_H
