#include "test.h"

#include <QEloquent/querybuilder.h>
#include <QEloquent/queryrunner.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QUrl>

using namespace QEloquent;

void Test::SetUp()
{
    connection = QEloquent::Connection::addConnection("DB", "QSQLITE", ":memory:");
    ASSERT_TRUE(connection.open()) << "Can't open connection";
}

void Test::TearDown()
{
    connection = Connection();
    QEloquent::Connection::removeConnection("DB");

    if (strcmp(TEST_DB_NAME, ":memory:") != 0)
        QFile::remove(TEST_DB_NAME);
}

std::expected<bool, QSqlError> Test::migrate()
{ return exec(QStringLiteral(TEST_DATA_DIR) + "/store/structure.sql"); }

std::expected<bool, QSqlError> Test::seed()
{ return exec(QStringLiteral(TEST_DATA_DIR) + "/store/content.sql"); }

std::expected<bool, QSqlError> Test::exec(const QString &sqlFileName)
{
    const QStringList statements = QEloquent::QueryBuilder::statementsFromScriptFile(sqlFileName);

    for (const QString &statement : statements) {
        auto result = QEloquent::QueryRunner::exec(statement, connection);
        if (!result)
            return std::unexpected(result.error());
    }

    return true;
}

QByteArray Test::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
        return file.readAll();

    return QByteArray();
}

std::list<std::string> Test::convertStringList(const QStringList &list)
{
    std::list<std::string> output;
    std::transform(list.begin(), list.end(), std::back_inserter(output), [](const QString &item) {
        return item.toStdString();
    });
    return output;
}
