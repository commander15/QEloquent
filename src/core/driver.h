#ifndef QELOQUENT_DRIVER_H
#define QELOQUENT_DRIVER_H

#include <QEloquent/global.h>

#include <QSqlDriver>

class QSqlDriver;

namespace QEloquent {

class QELOQUENT_EXPORT Driver : public QSqlDriver
{
    Q_OBJECT

public:
    enum FieldType {
        PrimaryKey,
        Raw,

        Boolean,

        Integer,
        BigInteger,
        SmallInteger,
        TinyInteger,

        Float,
        Double,
        Decimal,

        Char,
        String,

        Text,
        MediumText,
        LongText,

        Date,
        Time,
        DateTime,
        Timestamp,

        Binary,
        Json,
    };

    Driver(QSqlDriver *qtDriver);
    virtual ~Driver() = default;

    virtual FieldType primaryKeyType(bool *positive) const = 0;
    virtual QString autoIncrementKeyword() const = 0;

    virtual QString timestampDefault() const = 0;

    virtual QString columnType(FieldType baseType, int length, int precision = 2) const;

    bool isOpen() const override final;
    bool beginTransaction() override final;
    bool commitTransaction() override final;
    bool rollbackTransaction() override final;
    QStringList tables(QSql::TableType tableType) const override final;
    QSqlIndex primaryIndex(const QString &tableName) const override final;
    QSqlRecord record(const QString &tableName) const override final;
    QString formatValue(const QSqlField &field, bool trimStrings) const override final;
    QString escapeIdentifier(const QString &identifier, IdentifierType type) const override final;
    QString sqlStatement(StatementType type, const QString &tableName, const QSqlRecord &rec, bool preparedStatement) const override final;
    QVariant handle() const override final;
    bool hasFeature(DriverFeature f) const override final;
    void close() override final;
    QSqlResult *createResult() const override final;
    bool open(const QString &db, const QString &user, const QString &password, const QString &host, int port, const QString &connOpts) override final;
    bool subscribeToNotification(const QString &name) override final;
    bool unsubscribeFromNotification(const QString &name) override final;
    QStringList subscribedToNotifications() const override final;
    bool isIdentifierEscaped(const QString &identifier, IdentifierType type) const override final;
    QString stripDelimiters(const QString &identifier, IdentifierType type) const override final;
    int maximumIdentifierLength(IdentifierType type) const override final;

    QSqlDriver *qtDriver() const { return m_driver; }

    static Driver *create(const QString &qtDriverName, QSqlDriver *qtDriver);

public slots:
    bool cancelQuery() override final;

private:
    QSqlDriver *m_driver;
};

} // namespace QEloquent

#endif // QELOQUENT_DRIVER_H
