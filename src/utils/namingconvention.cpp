#include "namingconvention.h"
#include "namingconvention_p.h"

namespace QEloquent {

QString NamingConvention::foreignPropertyName(const QString &primaryPropertyName, const QString &className) const
{
    return camelFromPascal(className) + pascalFromCamel(primaryPropertyName);
}

NamingConvention *NamingConvention::convention(const QString &name)
{
    return s_conventions.value(name, s_conventions.value(name, nullptr));
}

QString NamingConvention::pascalFromCamel(const QString &str)
{
    QString camel = str;
    if (!camel.isEmpty())
        camel[0] = camel.at(0).toUpper();
    return camel;
}

QString NamingConvention::pascalFromSnake(const QString &str)
{
    if (str.isEmpty()) return str;

    QString result;
    result.reserve(str.size() + 5);

    bool capitalizeNext = true;
    for (QChar ch : str) {
        if (ch == '_') {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            result += ch.toUpper();
            capitalizeNext = false;
        } else {
            result += ch.toLower();
        }
    }

    // First character is always upper in PascalCase
    if (!result.isEmpty()) {
        result[0] = result[0].toUpper();
    }
    return result;
}

QString NamingConvention::camelFromPascal(const QString &str)
{
    QString pascal = str;
    if (!pascal.isEmpty()) {
        pascal[0] = pascal[0].toLower();
    }
    return pascal;
}

QString NamingConvention::camelFromSnake(const QString &str)
{
    QString pascal = pascalFromSnake(str);
    if (!pascal.isEmpty()) {
        pascal[0] = pascal[0].toLower();
    }
    return pascal;
}

QString NamingConvention::snakeFromPascal(const QString &str)
{
    if (str.isEmpty()) return str;

    QString result;
    result.reserve(str.size() + 10);

    for (int i = 0; i < str.size(); ++i) {
        QChar ch = str[i];

        // Detect uppercase → insert underscore before it (except first char)
        if (ch.isUpper()) {
            if (i > 0) {
                // Avoid double underscore on consecutive capitals: HTTPRequest → http_request
                if (i + 1 < str.size() && str[i + 1].isLower()) {
                    result += '_';
                } else if (i > 0 && !str[i - 1].isUpper()) {
                    result += '_';
                }
            }
            result += ch.toLower();
        }
        // Numbers also trigger a separator
        else if (ch.isDigit()) {
            if (i > 0 && !str[i - 1].isDigit()) {
                result += '_';
            }
            result += ch;
        }
        else {
            result += ch.toLower();
        }
    }
    return result;
}

QString NamingConvention::snakeFromCamel(const QString &str)
{
    if (str.isEmpty()) return str;

    QString result;
    result.reserve(str.size() + 10);

    for (int i = 0; i < str.size(); ++i) {
        QChar ch = str[i];

        if (ch.isUpper()) {
            // Always insert underscore before uppercase in camelCase (except first char)
            if (i > 0) {
                result += '_';
            }
            result += ch.toLower();
        }
        else if (ch.isDigit()) {
            if (i > 0 && !str[i - 1].isDigit()) {
                result += '_';
            }
            result += ch;
        }
        else {
            result += ch;
        }
    }
    return result;
}

NamingConvention::ConventionList NamingConvention::s_conventions = {
    { QStringLiteral("Laravel"), new LaravelNamingConvention() },
    { QStringLiteral("One One"), new OneOneNamingConvention() },
};

} // QEloquent namespace
