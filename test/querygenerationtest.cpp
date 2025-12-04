#include "querygenerationtest.h"

#include <QEloquent/modelquery.h>
#include <QEloquent/modelinfo.h>
#include <QEloquent/querybuilder.h>

#include <QVariantMap>

using namespace QEloquent;

TEST_F(QueryGenerationTest, selectGenerationTest)
{
    ModelQuery query;

    const QString statement1 = QueryBuilder::selectStatement(query, productModel);
    ASSERT_EQ(TEST_STR(statement1), "SELECT * FROM \"Products\"");

    query.where("name", "LIKE", "A%").orWhere("description", "LIKE", "T%");
    const QString statement2 = QueryBuilder::selectStatement(query, productModel);
    ASSERT_EQ(TEST_STR(statement2), "SELECT * FROM \"Products\" WHERE \"name\" LIKE 'A%' OR \"description\" LIKE 'T%'");
}

TEST_F(QueryGenerationTest, insertGeneration)
{
    QVariantMap data = {
        { "name", "Apple" },
    };

    const QString statement1 = QueryBuilder::insertStatement(data, productModel);
    ASSERT_EQ(TEST_STR(statement1), "INSERT INTO \"Products\" (\"name\") VALUES ('Apple')");

    data.insert("description", "Red fruit");
    data.insert("price", 1.5);
    const QString statement2 = QueryBuilder::insertStatement(data, productModel);
    ASSERT_EQ(TEST_STR(statement2), "INSERT INTO \"Products\" (\"description\", \"name\", \"price\") VALUES ('Red fruit', 'Apple', 1.5)");
}

TEST_F(QueryGenerationTest, updateGeneration)
{
    ModelQuery query;
    query.where("id", 1);

    QVariantMap data = {
      { "name", "Apple" },
    };

    const QString statement1 = QueryBuilder::updateStatement(query, data, productModel);
    ASSERT_EQ(TEST_STR(statement1), "UPDATE \"Products\" SET \"name\" = 'Apple' WHERE \"id\" = 1");

    data.insert("description", "Red fruit");
    data.insert("price", 1.5);
    const QString statement2 = QueryBuilder::updateStatement(query, data, productModel);
    ASSERT_EQ(TEST_STR(statement2), "UPDATE \"Products\" SET \"description\" = 'Red fruit', \"name\" = 'Apple', \"price\" = 1.5 WHERE \"id\" = 1");
}

TEST_F(QueryGenerationTest, deleteGeneration)
{
    ModelQuery query;
    const QString statement1 = QueryBuilder::deleteStatement(query, productModel);
    ASSERT_EQ(TEST_STR(statement1), "DELETE FROM \"Products\"");

    query.where("id", 1);
    const QString statement2 = QueryBuilder::deleteStatement(query, productModel);
    ASSERT_EQ(TEST_STR(statement2), "DELETE FROM \"Products\" WHERE \"id\" = 1");
}
