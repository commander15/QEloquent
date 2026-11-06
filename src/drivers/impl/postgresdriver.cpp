#include "postgresdriver_p.h"

namespace QEloquent {

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

QEloquent::PostgresGrammar::PostgresGrammar(Driver *driver) 
    : SchemaGrammar(driver)
{}

bool PostgresGrammar::hasFeature(GrammarFeature) const
{
    return true;
}

QString PostgresGrammar::autoIncrementKeyword() const
{
    return QStringLiteral("AUTO_INCREMENT");
}

QPair<SchemaGrammar::ColumnType, SchemaGrammar::NumberSign> PostgresGrammar::primaryKeyType() const
{
    return { ColumnData::BigInteger, ColumnData::Unsigned };
}

#endif

} // namespace QEloquent