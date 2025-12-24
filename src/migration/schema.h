#ifndef QELOQUENT_SCHEMA_H
#define QELOQUENT_SCHEMA_H

#include <QEloquent/global.h>
#include <QEloquent/tableblueprint.h>

#include <QException>

#include <functional>

class QSqlQuery;
class QSqlError;

namespace QEloquent {

class TableBlueprint;
class Connection;

class QELOQUENT_EXPORT Schema
{
public:
    static bool exists(const QString &tableName);
    static void create(const QString &tableName, const std::function<void(TableBlueprint &)> &callback);
    static void table(const QString &tableName, const std::function<void(TableBlueprint &)> &callback);
    static void drop(const QString &tableName);
    static void dropIfExists(const QString &tableName);

    static QString connectionName();
    static Connection connection();
    static void setConnection(const QString &name);

private:
    static QSqlQuery exec(const QString &statement);

    static QString s_connectionName;
};

class QELOQUENT_EXPORT SchemaException : public QException
{
public:
    SchemaException(const QString &statement, const QString &error)
        : statement(statement)
        , error(error)
        , m_what(statement.toUtf8() + '\n' + error.toUtf8())
    {}

    const char *what() const noexcept override
    { return m_what.data(); }

    void raise() const override { throw *this; }
    SchemaException *clone() const override { return new SchemaException(*this); }

    const QString statement;
    const QString error;

private:
    const QByteArray m_what;
};

} // namespace QEloquent

#endif // QELOQUENT_SCHEMA_H
