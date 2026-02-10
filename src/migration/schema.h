#ifndef QELOQUENT_SCHEMA_H
#define QELOQUENT_SCHEMA_H

#include <QEloquent/global.h>
#include <QEloquent/tableblueprint.h>

#include <QException>
#include <QSqlError>

class QSqlQuery;
class QSqlError;

namespace QEloquent {

class TableBlueprint;
class Connection;
class SchemaGrammar;

class QELOQUENT_EXPORT Schema
{
public:
    typedef std::function<void(TableBlueprint &)> BlueprintCallback;
    typedef std::function<void()> DefaultCallback;

    static bool hasTable(const QString &table);
    static void create(const QString &table, const BlueprintCallback &callback);
    static void table(const QString &table, const BlueprintCallback &callback);
    static void rename(const QString &from, const QString &to);
    static void drop(const QString &table);
    static void dropIfExists(const QString &table);

    static bool hasColumn(const QString &column, const QString &table);
    static bool hasColumns(const QStringList &columns, const QString &table);
    static void whenHasColumn(const QString &column, const QString &table, const DefaultCallback &callback);
    static void whenDoesntHaveColumn(const QString &column, const QString &table, const DefaultCallback &callback);
    static void dropColumn(const QString &column, const QString &table);
    static void dropColumns(const QStringList &columns, const QString &table);

    static QSqlQuery exec(const QString &statement);
    static QList<QSqlQuery> exec(const QStringList &statements);

    static QString connectionName();
    static Connection connection();
    static void setConnection(const QString &name);

private:
    static SchemaGrammar *grammar();

    static QString s_connectionName;

    friend class Migration;
};

class QELOQUENT_EXPORT SchemaException : public QException
{
public:
    SchemaException(const QString &statement, const QSqlError &error);

    const char *what() const noexcept override;

    void raise() const override;
    SchemaException *clone() const override;

    const QString statement;
    const QSqlError error;

private:
    const QByteArray m_what;
};

} // namespace QEloquent

#endif // QELOQUENT_SCHEMA_H
