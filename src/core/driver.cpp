#include "driver.h"
#include "driver_p.h"

#include <QSqlDriver>

namespace QEloquent {

Driver::Driver(QSqlDriver *qtDriver)
    : m_driver(qtDriver)
{}

QString Driver::columnType(FieldType baseType, int length) const
{
    switch (baseType) {
    case Boolean:
        return QStringLiteral("BOOLEAN");

    case Integer:
        return QStringLiteral("INTEGER");

    case Double:
        return QStringLiteral("DOUBLE");

    case Char:
        return QStringLiteral("CHAR(%1)").arg(length);

    case Varchar:
        return QStringLiteral("VARCHAR(%1)").arg(length);

    case Text:
        return QStringLiteral("TEXT");

    case Timestamp:
        return QStringLiteral("TIMESTAMP");
    }

    return QString();
}

Driver *Driver::create(const QString &qtDriverName, QSqlDriver *qtDriver)
{
    if (qtDriverName == QStringLiteral("QSQLITE"))
        return new SQLiteDriver(qtDriver);

    return new DefaultDriver(qtDriver);
}

} // namespace QEloquent
