#include "smartmodel.h"

#include "models/smartmodels.h"

TEST_F(SmartModel, getTest)
{
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    int productId = 1;
    QString productName = "Apple";
    QString productDescription = "Fresh red apple";
    double productPrice = 0.5;
    QString productBarcode = "1234567890123";
    int categoryId = 1;

    auto p = SmartProduct::find(productId);
    ASSERT_TRUE(p) << TEST_STR(p ? "" : p.error().message());

    QEloquent::ID id = p->id();
    ASSERT_EQ(id.format(), QEloquent::AbstractData::DataFormat::IntegerData);
    ASSERT_EQ(id.integer(), productId);
    ASSERT_EQ(p->id<int>(), productId);
    ASSERT_EQ(p->id<qint64>(), productId);

    QEloquent::Data<QString> name = p->name;
    ASSERT_FALSE(name.isNull());
    ASSERT_EQ(TEST_STR(name.get()), productName);
    ASSERT_EQ(TEST_STR(name), productName);
    ASSERT_EQ(TEST_STR(p->name), productName);

    ASSERT_EQ(TEST_STR(p->description), productDescription);
    ASSERT_EQ(p->price, productPrice);
    ASSERT_EQ(TEST_STR(p->barcode), productBarcode);
    ASSERT_EQ(p->property("categoryId").toInt(), categoryId);

    ASSERT_TRUE(p->createdAt().isValid());
}