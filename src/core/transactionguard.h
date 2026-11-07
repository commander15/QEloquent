#ifndef QELOQUENT_TRANSACTIONGUARD_H
#define QELOQUENT_TRANSACTIONGUARD_H

#include <QEloquent/global.h>

#include <QString>

namespace QEloquent {

class QELOQUENT_EXPORT TransactionGuard
{
public:
    TransactionGuard();
    TransactionGuard(const QString &connection);
    ~TransactionGuard();

    bool transactionStarted() const;
    QString errorCode() const;
    QString errorString() const;

    bool commit();

private:
    Q_DISABLE_COPY_MOVE(TransactionGuard)

    QString m_connection;
    bool m_started;
    bool m_commit;
    QString m_errorCode;
    QString m_errorString;
};

} // namespace QEloquent

#endif // QELOQUENT_TRANSACTIONGUARD_H
