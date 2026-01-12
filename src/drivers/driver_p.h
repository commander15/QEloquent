#ifndef QELOQUENT_DRIVER_P_H
#define QELOQUENT_DRIVER_P_H

#include "driver.h"

#ifdef QELOQUENT_MIGRATIONS_SUPPORT
#   include <QEloquent/private/schemagrammar_p.h>
#endif

#include <QString>

namespace QEloquent {

class DriverPrivate final
{
public:
    DriverPrivate(QSqlDriver *qtDriver) : sql(qtDriver) {}

    QSqlDriver *sql;
};

class DefaultDriver final : public Driver
{
public:
#ifndef QELOQUENT_MIGRATIONS_SUPPORT
    DefaultDriver(QSqlDriver *driver) : Driver(driver) {}
#else
    DefaultDriver(QSqlDriver *driver) : Driver(driver), m_grammar(this) {}

    SchemaGrammar *schemaGrammar() const override {
        return &m_grammar;
    }

private:
    mutable DefaultSchemaGrammar m_grammar;
#endif
};

}

#endif // QELOQUENT_DRIVER_P_H
