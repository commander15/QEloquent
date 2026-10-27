#include "postgresdriver_p.h"

namespace QEloquent {

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

} // namespace QEloquent