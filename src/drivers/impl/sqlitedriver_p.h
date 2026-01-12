#ifndef QELOQUENT_SQLITEDRIVER_P_H
#define QELOQUENT_SQLITEDRIVER_P_H

#include <QEloquent/driver.h>
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
#   include <QEloquent/schemagrammar.h>
#endif

namespace QEloquent {

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

class SQLiteGrammar final : public SchemaGrammar
{
public:
    SQLiteGrammar(Driver *driver);

    bool hasFeature(GrammarFeature feature) const override;

    QPair<ColumnType, NumberSign> primaryKeyType() const override;
    QString autoIncrementKeyword() const override;
    QString columnType(ColumnType type, int, int) const override;

    ColumnData::Constraints inlineConstraints() const override;
    ColumnData::Constraints namedConstraints() const override;
};

#endif

class SQLiteDriver final : public Driver
{
public:
    SQLiteDriver(QSqlDriver *qtDriver)
#ifndef QELOQUENT_MIGRATIONS_SUPPORT
        : Driver(qtDriver) {}
#else
        : Driver(qtDriver), m_grammar(this) {}

    SchemaGrammar *schemaGrammar() const override {
        return &m_grammar;
    }

private:
    mutable SQLiteGrammar m_grammar;
#endif
};

} // namespace QEloquent

#endif // QELOQUENT_SQLITEDRIVER_P_H
