#ifndef QELOQUENT_NAMINGCONVENTION_P_H
#define QELOQUENT_NAMINGCONVENTION_P_H

#include "namingconvention.h"

#include <QEloquent/dictionary.h>

namespace QEloquent {

class LaravelNamingConvention final : public NamingConvention
{
public:
    QString tableName(const QString &className) const override
    { return snakeFromPascal(Dictionary::plural(className)); }

    QString fieldName(const QString &propertyName, const QString &) const override
    { return snakeFromCamel(propertyName); }

    QString primaryFieldName(const QString &) const override
    { return QStringLiteral("id"); }

    QString foreignFieldName(const QString &primaryFieldName, const QString &tableName) const override
    { return tableName + '_' + primaryFieldName; }

    QString pivotTableName(const QString &table1, const QString &table2) const override
    { return table1 + '_' + table2; }

    QString propertyName(const QString &fieldName, const QString &) const override
    { return camelFromSnake(fieldName); }
};

class OneOneNamingConvention final : public NamingConvention
{
public:
    QString tableName(const QString &className) const override
    { return Dictionary::plural(className); }

    QString fieldName(const QString &propertyName, const QString &) const override
    { return propertyName; }

    QString primaryFieldName(const QString &) const override
    { return QStringLiteral("id"); }

    QString foreignFieldName(const QString &primaryFieldName, const QString &tableName) const override
    { return tableName + '_' + primaryFieldName; }

    QString pivotTableName(const QString &table1, const QString &table2) const override
    { return table1 + '_' + table2; }

    QString propertyName(const QString &fieldName, const QString &) const override
    { return fieldName; }
};

} // QEloquent namespace

#endif // QELOQUENT_NAMINGCONVENTION_P_H
