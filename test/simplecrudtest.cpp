#include "simplecrudtest.h"

#include <QEloquent/genericmodel.h>
#include <QEloquent/queryrunner.h>

#include <QVariant>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace QEloquent;

TEST_F(SimpleCRUDTest, metadataRetrieval)
{
    auto migrationResult = migrate();
    ASSERT_TRUE(migrationResult) << "Migration failed";

    RegisterProductModel(true);

    const std::list<std::string> fillables = {
        "name", "description", "price", "barcode", "category_id"
    };
    const std::list<std::string> fields = {
        "id", "name", "description", "price", "barcode",
        "category_id", "created_at", "updated_at"
    };

    const ModelInfo product = connection.modelInfo("Products");
    ASSERT_EQ(TEST_STR(product.table()), "Products");
    ASSERT_EQ(TEST_STR(product.primaryKey()), "id");
    ASSERT_EQ(TEST_STR(product.foreignKey()), "product_id");
    ASSERT_EQ(TEST_STR_LIST(product.hidden()), std::list<std::string>());
    ASSERT_EQ(TEST_STR_LIST(product.fillables()), fillables);
    ASSERT_EQ(TEST_STR_LIST(product.fields()), fields);
}

TEST_F(SimpleCRUDTest, retrievingSingleModel)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Generic<"Products">::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    const GenericModel product = result.value();
    ASSERT_EQ(TEST_STR(product.value("name").toString()), "Apple");
    ASSERT_EQ(TEST_STR(product.value("description").toString()), "Fresh red apple");
    ASSERT_EQ(product.value("price").toDouble(), 0.50);
    ASSERT_EQ(TEST_STR(product.value("barcode").toString()), "1234567890123");
    ASSERT_EQ(product.value("category_id").toInt(), 1);
}

TEST_F(SimpleCRUDTest, creatingSingleModel)
{
    auto migrationResult = migrate();
    ASSERT_TRUE(migrationResult) << "Migration failed";

    // Filling data
    QJsonObject object;
    object.insert("name", "Orange");
    object.insert("description", "Fresh oranges");
    object.insert("price", 50.0);
    object.insert("category_id", 1);

    // Storing data
    auto result = Generic<"Products">::create(object);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    // Cheking if data had been stored though SQL
    QString statement = "SELECT * FROM Products WHERE id = 1";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_TRUE(response->next());

    // Field by field check
    ASSERT_EQ(response->value("id").toInt(), 1);
    ASSERT_EQ(TEST_STR(response->value("name").toString()), TEST_STR(object.value("name").toString()));
    ASSERT_EQ(TEST_STR(response->value("description").toString()), TEST_STR(object.value("description").toString()));
    ASSERT_EQ(response->value("price").toDouble(), object.value("price").toDouble());
    ASSERT_EQ(response->value("category_id").toInt(), object.value("category_id").toInt());
}

TEST_F(SimpleCRUDTest, editSingleModel)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    // Retrieving first
    auto result = Generic<"Products">::find(1);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    // Filling data
    QJsonObject object;
    object.insert("id", 1);
    object.insert("name", "Orange");
    object.insert("description", "Fresh oranges");

    // Storing
    Model product = result.value();
    product.fill(object);
    ASSERT_TRUE(product.save()) << (product.exists() ? "" : TEST_STR(product.lastError().text()));

    // Cheking if data had been stored though SQL
    QString statement = "SELECT * FROM Products WHERE id = 1";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_TRUE(response->next());

    // Field by field check
    ASSERT_EQ(response->value("id").toInt(), 1);
    ASSERT_EQ(TEST_STR(response->value("name").toString()), TEST_STR(object.value("name").toString()));
    ASSERT_EQ(TEST_STR(response->value("description").toString()), TEST_STR(object.value("description").toString()));
}


TEST_F(SimpleCRUDTest, deleteSingleModel)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    ModelQuery query;
    query.where("id", 1);

    auto result = Generic<"Products">::remove(query);
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));

    ASSERT_EQ(result.value(), 1);

    // Checking though SQL
    QString statement = "SELECT * FROM Products WHERE id = 1";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_FALSE(response->next());
}

TEST_F(SimpleCRUDTest, countTest)
{
    auto migrationResult = migrate();
    auto seedingResult = seed();
    ASSERT_TRUE(migrationResult && seedingResult) << "Migration/Seeding failed";

    auto result = Generic<"Products">::count();
    ASSERT_TRUE(result) << (result ? "" : TEST_STR(result.error().text()));
    ASSERT_EQ(result.value(), 3);

    // Checking though SQL
    QString statement = "SELECT COUNT(id) FROM Products";
    auto response = QueryRunner::exec(statement, connection);
    ASSERT_TRUE(response) << (response ? "" : TEST_STR(response.error().text()));
    ASSERT_TRUE(response->next());
    ASSERT_EQ(response->value(0).toInt(), 3);
}
