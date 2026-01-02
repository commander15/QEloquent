#include "serialization.h"

TEST_F(Serialization, SimpleModelProducesValidJsonObject) {
    // Migration and seeding
    ASSERT_TRUE(migrateAndSeed()) << lastErrorText;

    auto result = SimpleProduct::find(1);
    ASSERT_TRUE(result) << TEST_STR(result ? "" : result.error().text());

    const QJsonObject object = result->toJsonObject();
    ASSERT_FALSE(object.contains("category_id")); // Hidden

    SimpleProduct apple;
    apple.id = object.value("id").toInt();
    apple.name = object.value("name").toString();
    apple.description = object.value("description").toString();
    apple.price = object.value("price").toDouble();
    apple.barcode = object.value("barcode").toString();
    apple.createdAt = QDateTime::fromString(object.value("created_at").toString(), Qt::ISODateWithMs);

    auto checkResult = isReallyAnApple(apple, [](SimpleProduct &p) {
        p.setProperty("categoryId", QVariant()); // Cause hidden
    });
    ASSERT_TRUE(checkResult) << (checkResult ? "" : TEST_STR(checkResult.error()));
}

TEST_F(Serialization, SqlRecordFillSimpleModelCompletely) {
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

TEST_F(Serialization, ValidJsonObjectFillSimpleModelPartially) {
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
