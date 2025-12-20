#ifndef TEST_H
#define TEST_H

#include <gtest/gtest.h>

#include <expected>
#include <list>
#include <string>

#include <QEloquent/connection.h>

#include <QSqlError>
#include <QStringList>

#define TEST_DB_NAME ":memory:"

#define TEST_STR(str)       str.toStdString()
#define TEST_STR_LIST(list) Test::convertStringList(list)

class Test : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    std::expected<bool, QSqlError> migrate();
    std::expected<bool, QSqlError> seed();
    std::expected<bool, QSqlError> exec(const QString &sqlFileName);

    static QByteArray readFile(const QString &fileName);

    static std::list<std::string> convertStringList(const QStringList &list);

    QEloquent::Connection connection;
};

#endif // TEST_H
