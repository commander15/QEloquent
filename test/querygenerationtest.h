#ifndef QUERYGENERATIONTEST_H
#define QUERYGENERATIONTEST_H

#include "core/mytest.h"

#include <QEloquent/metaobject.h>

class QueryGenerationTest : public MyTest
{
protected:
    void SetUp() override
    {
        MyTest::SetUp();
        ASSERT_TRUE(migrate());
    }

    QEloquent::MetaObject productMetaObject;
};

#endif // QUERYGENERATIONTEST_H
