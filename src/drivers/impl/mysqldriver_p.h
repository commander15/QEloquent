#ifndef QELOQUENT_MYSQLDRIVER_H
#define QELOQUENT_MYSQLDRIVER_H

#include <QEloquent/driver.h>
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
#   include <QEloquent/schemagrammar.h>
#endif

namespace QEloquent {

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

class MySQLGrammar final : public SchemaGrammar
{
public:
    MySQLGrammar(Driver *driver);

    bool hasFeature(GrammarFeature feature) const override;

    QPair<ColumnType, NumberSign> primaryKeyType() const override;
    QString autoIncrementKeyword() const override;
};

#endif

class MySQLDriver : public QEloquent::Driver
{
public:
    MySQLDriver(QSqlDriver *qtDriver)
        : Driver(qtDriver)
#ifdef QELOQUENT_MIGRATIONS_SUPPORT
        , m_grammar(this)
#endif
    {}

#ifdef QELOQUENT_MIGRATIONS_SUPPORT
    SchemaGrammar *schemaGrammar() const override
    { return &m_grammar; }

    mutable MySQLGrammar m_grammar;
#endif
};

} // namespace QEloquent

#endif // QELOQUENT_MYSQLDRIVER_H
