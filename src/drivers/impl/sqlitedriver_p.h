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
        : Driver(qtDriver)
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
        , m_grammar(this)
#endif
    {}

#ifdef QELOQUENT_MIGRATIONS_SUPPORT
    SchemaGrammar *schemaGrammar() const override
    { return &m_grammar; }

    mutable SQLiteGrammar m_grammar;
#endif
};

} // namespace QEloquent

#endif // QELOQUENT_SQLITEDRIVER_P_H
