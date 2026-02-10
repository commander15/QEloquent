#include "columndata.h"

#include <QEloquent/private/tableblueprint_p.h>

namespace QEloquent {

/*!
 * \class ColumnData
 * \brief Read-only snapshot of a column's definition after blueprint processing.
 *
 * ColumnData provides an immutable view of a column's properties, type, constraints,
 * defaults, foreign key references, indexes, and check expressions as defined in
 * the TableBlueprint.
 *
 * Instances are created internally by TableBlueprint and passed to SchemaGrammar
 * for final SQL generation. They are also exposed via TableData for introspection.
 *
 * \note This class is not intended for direct instantiation or modification by users.
 *       It is a data container only — all fluent configuration happens in ColumnDefinition.
 *
 * \sa ColumnDefinition for the mutable/fluent builder.
 * \sa TableBlueprint for creating columns.
 * \sa SchemaGrammar for using ColumnData to generate SQL.
 */

/*!
 * \enum ColumnData::ColumnType
 * \brief Defines the supported column data types in QEloquent.
 *
 * This enumeration maps high-level, portable column types to database-specific types
 * during SQL generation. The actual SQL type is determined by the active SchemaGrammar.
 *
 * Most types are self-explanatory and correspond directly to common SQL types.
 */

/*!
 * \var ColumnData::Raw
 * \brief Raw/custom SQL type.
 *
 * Used for database-specific or complex column definitions that bypass fluent typing.
 */

/*!
 * \var ColumnData::Boolean
 * \brief Boolean/TRUE/FALSE type.
 */

/*!
 * \var ColumnData::Integer
 * \brief Standard 32-bit integer.
 */

/*!
 * \var ColumnData::BigInteger
 * \brief 64-bit integer (BIGINT).
 */

/*!
 * \var ColumnData::SmallInteger
 * \brief 16-bit integer (SMALLINT).
 */

/*!
 * \var ColumnData::TinyInteger
 * \brief 8-bit integer (TINYINT).
 */

/*!
 * \var ColumnData::Float
 * \brief Single-precision floating point.
 */

/*!
 * \var ColumnData::Double
 * \brief Double-precision floating point.
 */

/*!
 * \var ColumnData::Decimal
 * \brief Fixed-point decimal number (with precision and scale).
 */

/*!
 * \var ColumnData::Char
 * \brief Fixed-length character string (CHAR).
 */

/*!
 * \var ColumnData::String
 * \brief Variable-length character string (VARCHAR).
 */

/*!
 * \var ColumnData::Text
 * \brief Large text field (TEXT).
 */

/*!
 * \var ColumnData::MediumText
 * \brief Medium-sized text field (MEDIUMTEXT).
 */

/*!
 * \var ColumnData::LongText
 * \brief Long text field (LONGTEXT).
 */

/*!
 * \var ColumnData::Date
 * \brief Date only (DATE).
 */

/*!
 * \var ColumnData::Time
 * \brief Time only (TIME).
 */

/*!
 * \var ColumnData::DateTime
 * \brief Date and time (DATETIME).
 */

/*!
 * \var ColumnData::Timestamp
 * \brief Timestamp with time zone support (TIMESTAMP).
 */

/*!
 * \var ColumnData::Binary
 * \brief Binary data (BLOB/BINARY).
 */

/*!
 * \var ColumnData::Json
 * \brief JSON data type (JSON/JSONB in supported databases).
 */

/*!
 * \enum ColumnData::NumberSign
 * \brief Specifies whether a numeric column is signed or unsigned.
 */

/*!
 * \var ColumnData::Signed
 * \brief Allows negative values (default for most numeric types).
 */

/*!
 * \var ColumnData::Unsigned
 * \brief Only non-negative values (maps to UNSIGNED in SQL).
 */

/*!
 * \enum ColumnData::ConstraintFlag
 * \brief Bit flags for column-level constraints.
 *
 * Used internally to track which constraints are applied to a column.
 * Flags can be combined using bitwise OR.
 */

/*!
 * \var ColumnData::NoConstraints
 * \brief No constraints applied (default).
 */

/*!
 * \var ColumnData::PrimaryKey
 * \brief Column is part of the primary key.
 */

/*!
 * \var ColumnData::AutoIncrement
 * \brief Column is auto-incrementing (usually for primary keys).
 */

/*!
 * \var ColumnData::Unique
 * \brief Column must contain unique values.
 */

/*!
 * \var ColumnData::Default
 * \brief Column has a default value or expression.
 */

/*!
 * \var ColumnData::NotNull
 * \brief Column does not allow NULL values.
 */

/*!
 * \var ColumnData::Check
 * \brief Column has one or more CHECK constraints.
 */

/*!
 * \var ColumnData::ForeignKey
 * \brief Column is part of a foreign key constraint.
 */

/*!
 * \typedef ColumnData::ForeignKeyAction
 * \brief Alias for ForeignKeyDefinition::ForeignKeyAction.
 *
 * Represents the possible actions on foreign key violation (ON UPDATE / ON DELETE).
 *
 * \sa ForeignKeyDefinition::ForeignKeyAction
 */

/*!
 * \brief Default constructor.
 *
 * Creates an empty column data object with default values.
 * Internal private data is initialized.
 */
ColumnData::ColumnData()
    : data(new ColumnDefinitionPrivate())
{}

/*!
 * \brief Constructs column data from shared private implementation.
 *
 * Used internally when sharing data with ColumnDefinition.
 *
 * \param d Shared pointer to existing private data.
 */
ColumnData::ColumnData(const QExplicitlySharedDataPointer<ColumnDefinitionPrivate> &d)
    : data(d)
{}

/*!
 * \brief Copy constructor.
 *
 * Creates a new column data object as a copy of another instance.
 * This is the default implementation (shared data copy).
 */
ColumnData::ColumnData(const ColumnData &other) = default;

/*!
 * \brief Move constructor.
 *
 * Creates a new column data object by moving from another instance.
 * This is the default implementation.
 */
ColumnData::ColumnData(ColumnData &&) = default;

/*!
 * \brief Copy assignment operator.
 *
 * Assigns the contents of another column data object to this instance.
 * This is the default implementation.
 *
 * \return Reference to this object.
 */
ColumnData &ColumnData::operator=(const ColumnData &) = default;

/*!
 * \brief Move assignment operator.
 *
 * Moves the contents of another column data object into this instance.
 * This is the default implementation.
 *
 * \return Reference to this object.
 */
ColumnData &ColumnData::operator=(ColumnData &&) = default;

/*!
 * \brief Destructor.
 *
 * Destroys this column data instance.
 * This is the default implementation.
 */
ColumnData::~ColumnData() = default;

/*!
 * \brief Returns the name of the column.
 *
 * \return The column name as defined in the blueprint.
 */
QString ColumnData::columnName() const
{
    return data->columnName;
}

/*!
 * \brief Returns the column's data type.
 *
 * \return The FieldType enum value (e.g., String, Integer, Timestamp).
 */
ColumnData::ColumnType ColumnData::columnType() const
{
    return data->type;
}

/*!
 * \brief Returns the column comment (if any).
 *
 * \return Comment text or empty string if none set.
 */
QString ColumnData::comment() const
{
    return data->comment;
}

/*!
 * \brief Returns the numeric sign constraint (signed/unsigned).
 *
 * \return NumberSign::Signed or NumberSign::Unsigned.
 */
ColumnData::NumberSign ColumnData::numberSign() const
{
    return data->numberSign;
}

/*!
 * \brief Returns the number of decimal places (for Decimal type).
 *
 * \return Scale value, or 0 if not applicable.
 */
int ColumnData::decimalPlaces() const
{
    return data->decimalPlaces;
}

/*!
 * \brief Returns the length/precision of the column.
 *
 * Meaning depends on type:
 * - String/Char: maximum length
 * - Decimal: total digits
 * - Others: may be unused
 *
 * \return Length value or -1 if not set.
 */
int ColumnData::length() const
{
    return data->length;
}

/*!
 * \brief Checks whether a specific constraint flag is set.
 *
 * \param constraint The ConstraintFlag to test (e.g., PrimaryKey, Unique).
 * \return true if the flag is set, false otherwise.
 */
bool ColumnData::hasConstraint(ConstraintFlag constraint) const
{
    return data->constraints.testFlag(constraint);
}

/*!
 * \brief Returns the literal default value (if set).
 *
 * Returns an invalid QVariant if the default is an expression or not set.
 *
 * \return Default literal value, or invalid QVariant if none or expression.
 *
 * \sa defaultExpression() for expression defaults.
 */
QVariant ColumnData::defaultValue() const
{
    return (data->defaultValueIsExpr ? QVariant() : data->defaultValue);
}

/*!
 * \brief Returns the default value as SQL expression (if set).
 *
 * If the default is a literal value, it is formatted using the driver.
 * If it's an expression, returns the raw expression.
 *
 * \param driver The active database driver (for literal formatting).
 * \return SQL expression for DEFAULT clause, or empty if none.
 */
QString ColumnData::defaultExpression(Driver *driver) const
{
    return (data->defaultValueIsExpr ? data->defaultValue.toString() : driver->formatValue(data->defaultValue));
}

/*!
 * \brief Returns all CHECK constraint expressions.
 *
 * Combines min/max/range/in/checkExpr into SQL CHECK expressions.
 *
 * \param driver The active database driver (for value formatting).
 * \return List of SQL CHECK expressions (may be empty).
 */
QStringList ColumnData::checks(Driver *driver) const
{
    QStringList checks;
    const QString &expr = QStringLiteral("%1 %2= %3")
                              .arg(driver->escapeFieldName(data->columnName));
    if (data->minValue.isValid())
        checks.append(expr.arg(">", driver->formatValue(data->minValue)));
    if (data->maxValue.isValid())
        checks.append(expr.arg("<", driver->formatValue(data->maxValue)));
    if (!data->inValues.isEmpty()) {
        QStringList values;
        std::transform(data->inValues.begin(), data->inValues.end(), std::back_inserter(values), [&driver] (const QVariant &value) {
            return driver->formatValue(value);
        });
        checks.append(driver->escapeFieldName(data->columnName) + " IN (" + values.join(", ") + ')');
    }
    checks.append(data->checkExpr);
    return checks;
}

/*!
 * \brief Returns the name of the index on this column (if any).
 *
 * \return Index name, or empty string if no index is defined.
 */
QString ColumnData::indexName() const
{
    return data->indexName;
}

/*!
 * \brief Returns the referenced table name for foreign keys.
 *
 * \return Referenced table name, or empty if not a foreign key.
 */
QString ColumnData::refTable() const
{
    return data->refTable;
}

/*!
 * \brief Returns the list of referenced columns for foreign keys.
 *
 * \return List of column names in the referenced table (usually one).
 */
QStringList ColumnData::refColumns() const
{
    return data->refColumns;
}

/*!
 * \brief Returns the ON UPDATE action for foreign keys.
 *
 * \return ForeignKeyAction value (NoAction, Cascade, etc.).
 */
ColumnData::ForeignKeyAction ColumnData::onUpdate() const
{
    return data->onUpdate;
}

/*!
 * \brief Returns the ON DELETE action for foreign keys.
 *
 * \return ForeignKeyAction value (NoAction, Cascade, etc.).
 */
ColumnData::ForeignKeyAction ColumnData::onDelete() const
{
    return data->onDelete;
}

/*!
 * \brief Returns the raw SQL definition (for raw columns only).
 *
 * \return Raw SQL string, or empty if not a raw column.
 */
QString ColumnData::rawDefinition() const
{
    return data->rawDefinition;
}

/*!
 * \brief Returns the name of the table this column belongs to.
 *
 * \return Table name.
 */
QString ColumnData::tableName() const
{
    return data->tableName;
}

} // namespace QEloquent
