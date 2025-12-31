#include "mytest.h"

#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QUrl>

using namespace QEloquent;

QByteArray MyTest::readFile(const QString &fileName)
{
    if (s_fileCache.contains(fileName))
        return s_fileCache.value(fileName);
    else
        return cacheFile(fileName);
}

QByteArray MyTest::cacheFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        const QByteArray buffer = file.readAll();
        s_fileCache.insert(fileName, buffer);
        return buffer;
    }

    return QByteArray();
}

bool MyTest::writeFile(const QString &fileName, const QByteArray &data)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.flush();
        file.close();
        return true;
    } else {
        return false;
    }
}

std::string MyTest::convertString(const QString &str)
{
    return str.toStdString();
}

std::list<std::string> MyTest::convertStringList(const QStringList &list)
{
    std::list<std::string> output;
    std::transform(list.begin(), list.end(), std::back_inserter(output), [](const QString &item) {
        return item.toStdString();
    });
    return output;
}

void MyTest::SetUp()
{
    connection = QEloquent::Connection::addConnection("DB", "QSQLITE", ":memory:");
    ASSERT_TRUE(connection.open()) << "Can't open connection";
}

void MyTest::TearDown()
{
    connection = Connection();
    QEloquent::Connection::removeConnection("DB");

    if (strcmp(TEST_DB_NAME, ":memory:") != 0)
        QFile::remove(TEST_DB_NAME);
}

QEloquent::Result<bool, QSqlError> MyTest::migrateAndSeed()
{
    auto migration = this->migrate();
    if (!migration.has_value()) return migration;
    else return this->seed();
}

Result<bool, QSqlError> MyTest::migrate()
{ return exec(QStringLiteral(TEST_DATA_DIR) + "/store/structure.sql"); }

Result<bool, QSqlError> MyTest::seed()
{ return exec(QStringLiteral(TEST_DATA_DIR) + "/store/content.sql"); }

Result<bool, QSqlError> MyTest::exec(const QString &sqlFileName)
{
    const QByteArray fileContent = readFile(sqlFileName);
    const QStringList statements = QEloquent::QueryBuilder::statementsFromScriptContent(fileContent);

    for (const QString &statement : statements) {
        auto result = QEloquent::QueryRunner::exec(statement, connection);
        if (!result)
            return unexpected(result.error());
    }

    return true;
}

QMap<QString, QByteArray> MyTest::s_fileCache;
