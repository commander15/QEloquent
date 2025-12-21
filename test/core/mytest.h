#ifndef MYTEST_H
#define MYTEST_H

#include <gtest/gtest.h>

#include <expected>
#include <list>
#include <string>

#include <QEloquent/result.h>
#include <QEloquent/connection.h>

#include <QSqlError>
#include <QStringList>
#include <QMap>

#define TEST_DB_NAME ":memory:"
//#define TEST_DB_NAME "database.sqlite"

#define TEST_STR(str)       MyTest::convertString(str)
#define TEST_STR_LIST(list) MyTest::convertStringList(list)

class MyTest : public testing::Test
{
public:
    static QByteArray readFile(const QString &fileName);
    static QByteArray cacheFile(const QString &fileName);
    static bool writeFile(const QString &fileName, const QByteArray &data);

    static std::string convertString(const QString &str);
    static std::list<std::string> convertStringList(const QStringList &list);

protected:
    void SetUp() override;
    void TearDown() override;

    QEloquent::Result<bool, QSqlError> migrate();
    QEloquent::Result<bool, QSqlError> seed();
    QEloquent::Result<bool, QSqlError> exec(const QString &sqlFileName);

    QEloquent::Connection connection;

private:
    static QMap<QString, QByteArray> s_fileCache;
};

#endif // MYTEST_H
