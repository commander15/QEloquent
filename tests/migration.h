#ifndef MIGRATION_H
#define MIGRATION_H

#include <mytest.h>

class Migration : public MyTest
{
protected:
    void SetUp() override;
    void TearDown() override;
};

#endif // MIGRATION_H
