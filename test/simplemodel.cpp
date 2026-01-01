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

TEST_F(SimpleModel, RetrieveValidInstancesForExistingRecords) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Retrieve apple, banana and milk
    auto result = SimpleProduct::all();
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));
    ASSERT_EQ(result->count(), 3);

    // Cheking results
    Result<bool, QString> checkResult;

    const SimpleProduct apple = result->at(0);
    checkResult = isReallyAnApple(apple);
    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));

    const SimpleProduct banana = result->at(1);
    checkResult = isReallyABanana(banana);
    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));

    const SimpleProduct milk = result->at(2);
    checkResult = isReallyMilk(milk);
    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));
}

TEST_F(SimpleModel, StoreValidInstanceToDB) {
    // Migration
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    // Saving
    SimpleProduct kivo;
    kivo.name = "Kivo";
    kivo.description = "Milky way";
    kivo.price = 2.5;
    kivo.barcode = "45648486454";
    kivo.setProperty("categoryId", 2); // Milky
    ASSERT_TRUE(kivo.save()) << TEST_STR(kivo.lastError().text());
    ASSERT_EQ(kivo.id, 4) << TEST_STR("Kivo got id " + QString::number(kivo.id));

    // Checking
    auto result = connection.exec("SELECT * FROM Products WHERE id = 4");
    ASSERT_TRUE(result) << TEST_STR(result ? "" : result.error().text());
    ASSERT_TRUE(result->next()) << "Kivo record not found";

    // Clone for comparison
    SimpleProduct clone;
    clone.fill(result->record());

    // Comparing
    auto comparison = isReally(clone, kivo);
    ASSERT_TRUE(comparison) << TEST_STR(comparison ? "" : comparison.error());
}

TEST_F(SimpleModel, UpdateValidInstanceOnDB) {
    // Migration
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto findResult = SimpleProduct::find(1);
    ASSERT_TRUE(findResult) << TEST_STR(findResult ? "" : findResult.error().text());

    SimpleProduct apple = findResult.value();
    apple.name = "Bio Apple"; // Changing name

    // Saving changes
    ASSERT_TRUE(apple.save()) << TEST_STR(findResult ? "" : findResult.error().text());

    // Checking
    auto result = connection.exec("SELECT name FROM Products WHERE id = 1");
    ASSERT_TRUE(result) << TEST_STR(result ? "" : result.error().text());
    ASSERT_TRUE(result->next()) << "Bio Apple not found";
    ASSERT_EQ(TEST_STR(result->value(0).toString()), "Bio Apple");
}

TEST_F(SimpleModel, DeleteValidInstanceOnDB) {
    // Migration
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto findResult = SimpleProduct::find(1);
    ASSERT_TRUE(findResult) << TEST_STR(findResult ? "" : findResult.error().text());

    SimpleProduct apple = findResult.value();
    ASSERT_TRUE(apple.deleteData()) << TEST_STR(findResult ? "" : findResult.error().text());

    // Checking
    auto result = connection.exec("SELECT id FROM Products WHERE id = 1");
    ASSERT_TRUE(result) << TEST_STR(result ? "" : result.error().text());
    ASSERT_FALSE(result->next()) << "Bio Apple still there found";
}

TEST_F(SimpleModel, DeleteValidInstancesOnDB) {
    // Migration
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    QEloquent::Query query;
    query.where("category_id", 1); // Fruits

    auto removeResult = SimpleProduct::remove(query);
    ASSERT_TRUE(removeResult) << TEST_STR(removeResult ? "" : removeResult.error().text());
    ASSERT_EQ(removeResult.value(), 2); // 2 record deleted

    // Checking
    auto result = connection.exec("SELECT COUNT(id) FROM Products WHERE category_id = 1");
    ASSERT_TRUE(result) << TEST_STR(result ? "" : result.error().text());
    ASSERT_FALSE(result->next()); // No records remain
}
