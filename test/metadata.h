#ifndef METADATA_H
#define METADATA_H

#include <mytest.h>

// Only for DB facilities
class MetaData : public MyTest
{
protected:
    void SetUp() override {
        MyTest::SetUp();

        auto result = migrate();
        ASSERT_TRUE(result);
    }
};

#endif // METADATA_H
