#include "sqlitedriver_p.h"

namespace QEloquent {

#ifdef QELOQUENT_MIGRATIONS_SUPPORT

SQLiteGrammar::SQLiteGrammar(Driver *driver)
    : SchemaGrammar(driver)
{}

bool SQLiteGrammar::hasFeature(GrammarFeature feature) const {
    switch (feature) {
    case SchemaGrammar::NumberSignSupport:
        return false;

    case SchemaGrammar::FullAlterColumnDefinitionSupport:
        return false;
    }

    return false;
}

QPair<SchemaGrammar::ColumnType, SchemaGrammar::NumberSign> SQLiteGrammar::primaryKeyType() const
{
    return { ColumnData::Integer, ColumnData::Signed };
}

QString SQLiteGrammar::autoIncrementKeyword() const {
    return QStringLiteral("AUTOINCREMENT");
}

QString SQLiteGrammar::columnType(ColumnType type, int, int) const {
    switch (type) {
    case ColumnType::Boolean:
    case ColumnType::Integer:
    case ColumnType::BigInteger:
    case ColumnType::SmallInteger:
    case ColumnType::TinyInteger:
        return QStringLiteral("INTEGER");

    case ColumnType::Double:
    case ColumnType::Float:
        return QStringLiteral("REAL");

    case ColumnType::Decimal:
        return QStringLiteral("NUMERIC");

    case ColumnType::Char:
    case ColumnType::String:
    case ColumnType::Text:
    case ColumnType::MediumText:
    case ColumnType::LongText:
    case ColumnType::Date:
    case ColumnType::Time:
    case ColumnType::DateTime:
    case ColumnType::Timestamp:
    case ColumnType::Json:
        return QStringLiteral("TEXT");

    case ColumnType::Binary:
        return QStringLiteral("BLOB");

    case ColumnType::Raw:
        return QString();
    }

    return QString();
}

ColumnData::Constraints SQLiteGrammar::inlineConstraints() const
{
    return  SchemaGrammar::inlineConstraints() | ColumnData::PrimaryKey;
}

ColumnData::Constraints SQLiteGrammar::namedConstraints() const
{
    return SchemaGrammar::namedConstraints() ^ ColumnData::PrimaryKey;
}

#endif

} // namespace QEloquent
