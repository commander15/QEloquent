#ifndef QELOQUENT_ERROR_H
#define QELOQUENT_ERROR_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>
#include <QSqlError>

class QSqlQuery;
class QSqlError;

namespace QEloquent {

class ErrorData;
/**
 * @brief Represents an error in a database operation.
 */
class QELOQUENT_EXPORT Error
{
public:
    /** @brief Error types */
    enum ErrorType {
        NoError,        /**< @brief Operation succeeded */
        NotFoundError,  /**< @brief Record requested by ID was not found */
        DatabaseError   /**< @brief Error returned by the SQL driver */
    };

    /** @brief Constructs a NoError object */
    Error();
    /** @brief Constructs an error with a type and message */
    Error(ErrorType error, const QString &text = QString(), const QSqlError &sqlError = QSqlError());
    /** @brief Copy constructor */
    Error(const Error &);
    /** @brief Move constructor */
    Error(Error &&);
    /** @brief Copy assignment operator */
    Error &operator=(const Error &);
    /** @brief Move assignment operator */
    Error &operator=(Error &&);
    /** @brief Destructor */
    ~Error();

    /** @brief Returns a human-readable error message */
    QString text() const;
    /** @brief Returns the underlying QSqlError */
    QSqlError sqlError() const;
    /** @brief Returns the error type */
    ErrorType type() const;

    /** @brief Creates an Error object from a QSqlQuery results */
    static Error fromSqlQuery(const QSqlQuery &query);
    /** @brief Creates an Error object from a QSqlError */
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
