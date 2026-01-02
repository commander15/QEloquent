#include <gtest/gtest.h>

#include <QCoreApplication>

#include <core/mytest.h>

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    QCoreApplication app(argc, argv);

    MyTest::cacheFile(QStringLiteral(TEST_DATA_DIR) + "/store/structure.sql");
    MyTest::cacheFile(QStringLiteral(TEST_DATA_DIR) + "/store/content.sql");

    return RUN_ALL_TESTS();
}
