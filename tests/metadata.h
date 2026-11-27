#ifndef METADATA_H
#define METADATA_H

#include <mytest.h>

// Only for DB facilities
class MetaData : public MyTest
{
protected:
    void SetUp() override {
        MyTest::SetUp();

        // No more needed since QEloquent didn't rely on DB to detect mappable fields anymore
        //auto result = migrate();
        //ASSERT_TRUE(result);
    }
};

#endif // METADATA_H
