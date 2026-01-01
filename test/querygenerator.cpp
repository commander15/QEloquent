#include "querygenerator.h"

#include <QEloquent/query.h>
#include <QEloquent/querybuilder.h>
#include <QEloquent/datamap.h>

using namespace QEloquent;

TEST_F(QueryGenerator, ValidQueryProducesValidSelectStatement) {
    Query query;
    query.table("Products");

    const QString statement1 = QueryBuilder::selectStatement(query);
    ASSERT_EQ(TEST_STR(statement1), "SELECT * FROM \"Products\"");

    const QString statement2 = QueryBuilder::selectStatement("SUM(count)", query);
    ASSERT_EQ(TEST_STR(statement2), "SELECT SUM(count) FROM \"Products\"");

    const QString statement3 = QueryBuilder::selectStatement(QStringList() << "id" << "name", query);
    ASSERT_EQ(TEST_STR(statement3), "SELECT \"id\", \"name\" FROM \"Products\"");

    const QList<QPair<QString, QString>> aliasedItems = { { "id", "productId" }, { "SUM(category_id)", "in_category"} };
    const QString statement4 = QueryBuilder::selectStatement(aliasedItems, query);
    ASSERT_EQ(TEST_STR(statement4), "SELECT id AS \"productId\", SUM(category_id) AS \"in_category\" FROM \"Products\"");

    query.where("name", "LIKE", "A%").orWhere("description", "LIKE", "T%");
    const QString statement5 = QueryBuilder::selectStatement(query);
    ASSERT_EQ(TEST_STR(statement5), "SELECT * FROM \"Products\" WHERE \"name\" LIKE 'A%' OR \"description\" LIKE 'T%'");
}

TEST_F(QueryGenerator, ValidQueryAndDataProducesValidInsertStatement) {
    DataMap data = {
        { "name", "Apple" },
    };

    Query query;
    query.table("Products");

    const QString statement1 = QueryBuilder::insertStatement(data, query);
    ASSERT_EQ(TEST_STR(statement1), "INSERT INTO \"Products\" (\"name\") VALUES ('Apple')");

    data.insert("description", "Red fruit");
    data.insert("price", 1.5);
    const QString statement2 = QueryBuilder::insertStatement(data, query);
    ASSERT_EQ(TEST_STR(statement2), "INSERT INTO \"Products\" (\"name\", \"description\", \"price\") VALUES ('Apple', 'Red fruit', 1.5)");
}

TEST_F(QueryGenerator, ValidQueryAndDataProducesValidUpdateStatement) {
    DataMap data = {
        { "name", "Apple" },
    };

    Query query;
    query.where("id", 1);
    query.table("Products");

    const QString statement1 = QueryBuilder::updateStatement(data, query);
    ASSERT_EQ(TEST_STR(statement1), "UPDATE \"Products\" SET \"name\" = 'Apple' WHERE \"id\" = 1");

    data.insert("description", "Red fruit");
    data.insert("price", 1.5);
    const QString statement2 = QueryBuilder::updateStatement(data, query);
    ASSERT_EQ(TEST_STR(statement2), "UPDATE \"Products\" SET \"name\" = 'Apple', \"description\" = 'Red fruit', \"price\" = 1.5 WHERE \"id\" = 1");
}

TEST_F(QueryGenerator, ValidQueryProducesValidDeleteStatement) {
    Query query;
    query.table("Products");

    const QString statement1 = QueryBuilder::deleteStatement(query);
    ASSERT_EQ(TEST_STR(statement1), "DELETE FROM \"Products\"");

    query.where("id", 1);
    const QString statement2 = QueryBuilder::deleteStatement(query);
    ASSERT_EQ(TEST_STR(statement2), "DELETE FROM \"Products\" WHERE \"id\" = 1");
}
