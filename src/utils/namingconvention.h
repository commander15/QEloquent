#ifndef QLOQUENT_NAMINGCONVENTION_H
#define QLOQUENT_NAMINGCONVENTION_H

#include <QEloquent/global.h>

#include <QMap>

namespace QEloquent {

class QELOQUENT_EXPORT NamingConvention
{
public:
    virtual ~NamingConvention() = default;

    virtual QString tableName(const QString &className) const = 0;
    virtual QString fieldName(const QString &propertyName, const QString &tableName) const = 0;

    virtual QString primaryFieldName(const QString &tableName) const = 0;
    virtual QString foreignFieldName(const QString &primaryFieldName, const QString &tableName) const = 0;

    virtual QString propertyName(const QString &fieldName, const QString &tableName) const = 0;
    virtual QString foreignPropertyName(const QString &primaryFieldName, const QString &tableName) const;

    static NamingConvention *convention(const QString &name = QStringLiteral("Laravel"));

    static QString pascalFromSnake(const QString &str);
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
