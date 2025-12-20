#ifndef QLOQUENT_NAMINGCONVENTION_H
#define QLOQUENT_NAMINGCONVENTION_H

#include <QEloquent/global.h>

#include <QMap>

namespace QEloquent {

/**
 * @brief Interface for naming conventions.
 * 
 * NamingConvention is responsible for translating between C++ class/property names
 * and database table/field names.
 */
class QELOQUENT_EXPORT NamingConvention
{
public:
    /** @brief Virtual destructor */
    virtual ~NamingConvention() = default;

    /** @brief Returns the table name for a given class name */
    virtual QString tableName(const QString &className) const = 0;
    /** @brief Returns the field name for a given property name in a table */
    virtual QString fieldName(const QString &propertyName, const QString &tableName) const = 0;

    /** @brief Returns the name of the primary key field for a table */
    virtual QString primaryFieldName(const QString &tableName) const = 0;
    /** @brief Returns the foreign key field name referring to another table */
    virtual QString foreignFieldName(const QString &primaryFieldName, const QString &tableName) const = 0;

    /** @brief Returns the property name for a given field name in a table */
    virtual QString propertyName(const QString &fieldName, const QString &tableName) const = 0;
    /** @brief Returns the foreign property name (e.g. for relations) */
    virtual QString foreignPropertyName(const QString &primaryPropertyName, const QString &className) const;

    /** @brief Retrieves a registered naming convention by name */
    static NamingConvention *convention(const QString &name = QStringLiteral("Laravel"));

    static QString pascalFromCamel(const QString &str);
    static QString pascalFromSnake(const QString &str);
    static QString camelFromPascal(const QString &str);
    static QString camelFromSnake(const QString &str);
    static QString snakeFromPascal(const QString &str);
    static QString snakeFromCamel(const QString &str);

private:
    class ConventionList final : public QMap<QString, NamingConvention *> {
    public:
        ConventionList(std::initializer_list<std::pair<QString, NamingConvention *>> list)
            : QMap(list) {}

        ~ConventionList() {
            QList<NamingConvention *> conventions = values();
            while (!conventions.isEmpty()) {
                NamingConvention *convention = conventions.takeFirst();
                conventions.removeAll(convention);
                delete convention;
            }
        }
    };

    static ConventionList s_conventions;
};

} // QEloquent namespace

#endif // QLOQUENT_NAMINGCONVENTION_H
