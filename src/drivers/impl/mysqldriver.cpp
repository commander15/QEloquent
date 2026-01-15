#include "mysqldriver_p.h"

namespace QEloquent {

QEloquent::MySQLGrammar::MySQLGrammar(Driver *driver) :
    SchemaGrammar(driver)
{}

bool MySQLGrammar::hasFeature(GrammarFeature) const
{
    return true;
}

QString MySQLGrammar::autoIncrementKeyword() const
{
    return QStringLiteral("AUTO_INCREMENT");
}

QPair<SchemaGrammar::ColumnType, SchemaGrammar::NumberSign> MySQLGrammar::primaryKeyType() const
{
    return { ColumnData::BigInteger, ColumnData::Unsigned };
}

} // namespace QEloquent
