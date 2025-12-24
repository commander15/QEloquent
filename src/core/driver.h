#ifndef QELOQUENT_DRIVER_H
#define QELOQUENT_DRIVER_H

#include <QEloquent/global.h>

class QSqlDriver;

namespace QEloquent {

class QELOQUENT_EXPORT Driver
{
public:
    enum FieldType {
        Boolean,
        Integer,
        Double,
        Char,
        Varchar,
        Text,
        Timestamp
    };

    Driver(QSqlDriver *qtDriver);
    virtual ~Driver() = default;

    virtual QString primaryKeyType(bool autoIncrement = true) const = 0;
    virtual QString autoIncrementKeyword() const = 0;

    virtual QString timestampDefault() const = 0;

    virtual QString columnType(FieldType baseType, int length) const;

    virtual bool supportsForeignKeys() const = 0;
    virtual QString foreignKeyConstraint(const QString& column,
                                         const QString& refTable,
                                         const QString& refColumn) const = 0;

    QSqlDriver *qtDriver() const { return m_driver; }

    static Driver *create(const QString &qtDriverName, QSqlDriver *qtDriver);

private:
    QSqlDriver *m_driver;
};

} // namespace QEloquent

#endif // QELOQUENT_DRIVER_H
