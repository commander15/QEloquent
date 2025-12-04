#ifndef QLOQUENT_NAMINGCONVENTION_H
#define QLOQUENT_NAMINGCONVENTION_H

#include <QEloquent/global.h>

namespace QEloquent {

class QELOQUENT_EXPORT NamingConvention
{
public:
    virtual ~NamingConvention() = default;

    virtual QString conventionName() const = 0;

    virtual QString tableName(const QString &className) const = 0;
    virtual QString fieldName(const QString &propertyName) const = 0;
    virtual QString propertyName(const QString &fieldName) const = 0;

    static NamingConvention *convention(const QString &name);
};

} // QEloquent namespace

#endif // QLOQUENT_NAMINGCONVENTION_H
