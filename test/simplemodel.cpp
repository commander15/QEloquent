#include "simplemodel.h"

TEST_F(SimpleModel, TestSqlDeserialization) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Running a simple SELECT query
    auto queryResult = connection.exec("SELECT * FROM Products");
    ASSERT_TRUE(queryResult) << "Error during query execution";
    ASSERT_TRUE(queryResult->next()) << "Can't find record, maybe migration/seed failed";

    // Serialize and check
    SimpleProduct product;
    product.fill(queryResult->record());
    auto checkResult = isReallyAnApple(product);
    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));
}

TEST_F(SimpleModel, TestJsonDeserialization) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Running a simple SELECT query
    auto queryResult = connection.exec("SELECT * FROM Products");
    ASSERT_TRUE(queryResult) << "Error during query execution";
    ASSERT_TRUE(queryResult->next()) << "Can't find record, maybe migration/seed failed";

    // Convert record to json object
    QJsonObject object;
    const QSqlRecord record = queryResult->record();
    for (int i(0); i < record.count(); ++i)
        object.insert(record.fieldName(i), QJsonValue::fromVariant(record.value(i)));

    // Serialize
    SimpleProduct product;
    product.fill(object);

    // Check that id and timestamps has not been set
    ASSERT_EQ(product.id, 0);
    ASSERT_FALSE(product.createdAt.isValid());
    ASSERT_FALSE(product.updatedAt.isValid());

    // Checking the rest
    auto checkResult = isReallyAnApple(product, [](SimpleProduct &p) {
        // Since json serialization doesn't support id and timestamps filling
        p.id = 0;
        p.createdAt = QDateTime();
        p.updatedAt = QDateTime();
    });

    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));
}

TEST_F(SimpleModel, RetrieveValidInstanceForExistingRecord) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Retrieve apple
    auto result = SimpleProduct::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    // Check it
    const SimpleProduct product = result.value();
    auto checkResult = isReallyAnApple(product);
    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));
}
