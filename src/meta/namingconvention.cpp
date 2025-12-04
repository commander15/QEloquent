#include "namingconvention.h"

#include <QString>

namespace QEloquent {

class LaravelNamingConvention : public NamingConvention
{
public:

    // NamingConvention interface
public:
    QString conventionName() const override { return QStringLiteral("Laravel"); }

    QString tableName(const QString &className) const override
    {
        QString table;

        bool first = true;
        for (const QChar &c : className) {
            if (first) {
                table.append(c.toLower());
                first = false;
            } else if (c.isUpper()) {
                table.append('_' + c.toLower());
            } else {
                table.append(c);
            }
        }

        return table;
    }

    QString fieldName(const QString &propertyName) const override
    {
        QString field;

        for (const QChar &c : propertyName) {
            if (c.isLower())
                field.append(c);
            else
                field.append('_' + c.toLower());
        }

        return field;
    }

    QString propertyName(const QString &fieldName) const override
    {
        QString property;

        bool upperNext = false;
        for (const QChar &c : fieldName) {
            if (c == '_') {
                upperNext = true;
            } else if (upperNext) {
                property.append(c.toUpper());
                upperNext = false;
            } else {
                property.append(c);
            }
        }

        return property;
    }
};

QEloquent::NamingConvention *QEloquent::NamingConvention::convention(const QString &name)
{
    static LaravelNamingConvention laravel;

    if (name == "Laravel")
        return &laravel;

    return nullptr;
}

} // QEloquent namespace
