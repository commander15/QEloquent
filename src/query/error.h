#ifndef QELOQUENT_ERROR_H
#define QELOQUENT_ERROR_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>
#include <QSqlError>

class QSqlQuery;
class QSqlError;

namespace QEloquent {

class ErrorData;
class QELOQUENT_EXPORT Error
{
public:
    enum ErrorType {
        NoError,
        NotFoundError,
        DatabaseError
    };

    Error();
    Error(ErrorType error, const QString &text = QString(), const QSqlError &sqlError = QSqlError());
    Error(const Error &);
    Error(Error &&);
    Error &operator=(const Error &);
    Error &operator=(Error &&);
    ~Error();

    QString text() const;
    QSqlError sqlError() const;
    ErrorType type() const;

    static Error fromSqlQuery(const QSqlQuery &query);
    static Error fromSqlError(const QSqlError &error);

private:
    Error(ErrorData *data);

    QSharedDataPointer<ErrorData> data;
};

} // namespace QEloquent

#ifndef QT_NO_DEBUG_STREAM
Q_SQL_EXPORT QDebug operator<<(QDebug, const QEloquent::Error &);
#endif

#endif // QELOQUENT_ERROR_H
