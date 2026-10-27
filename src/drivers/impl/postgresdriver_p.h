#ifndef QELOQUENT_POSTGRESDRIVER_H
#define QELOQUENT_POSTGRESDRIVER_H

#include <QEloquent/driver.h>
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
#   include <QEloquent/schemagrammar.h>
#endif

namespace QEloquent {

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

class PostgresGrammar final : public SchemaGrammar
{
public:
    PostgresGrammar(Driver *driver);

    bool hasFeature(GrammarFeature feature) const override;

    QPair<ColumnType, NumberSign> primaryKeyType() const override;
    QString autoIncrementKeyword() const override;
};

#endif

class PostgresDriver final : public QEloquent::Driver
{
public:
    PostgresDriver(QSqlDriver *qtDriver)
        : Driver(qtDriver)
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
        , m_grammar(this)
#endif
    {}

#ifdef QELOQUENT_MIGRATIONS_SUPPORT
    SchemaGrammar *schemaGrammar() const override
    { return &m_grammar; }

    mutable PostgresGrammar m_grammar;
#endif
};

} // namespace QEloquent

#endif // QELOQUENT_POSTGRESDRIVER_H