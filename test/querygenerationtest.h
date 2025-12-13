#ifndef QUERYGENERATIONTEST_H
#define QUERYGENERATIONTEST_H

#include "test.h"

#include <QEloquent/metaobject.h>

class QueryGenerationTest : public Test
{
protected:
    void SetUp() override
    {
        Test::SetUp();
        ASSERT_TRUE(migrate());
    }

    QEloquent::MetaObject productMetaObject;
};

#endif // QUERYGENERATIONTEST_H
