#include "error.h"

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

class ErrorData : public QSharedData
{
public:
    QString info;
    QString text;
    QSqlError sqlError;
    Error::ErrorType type = Error::NoError;
};

/*!
 * \class QEloquent::Error
 * \brief Represents an error in a database operation.
 */

/*!
 * \brief Constructs a NoError object.
 */
Error::Error()
    : data(new ErrorData)
{
}

/*!
 * \brief Constructs an error with a type and message.
 */
Error::Error(ErrorType error, const QString &text, const QSqlError &sqlError)
    : data(new ErrorData)
{
    data->type = error;
    data->text = text;
    data->sqlError = sqlError;
}

Error::Error(ErrorData *data)
    : data(data)
{
}

/*!
 * \brief Copy constructor.
 */
Error::Error(const Error &rhs)
    : data{rhs.data}
{}

/*!
 * \brief Move constructor.
 */
Error::Error(Error &&rhs)
    : data{std::move(rhs.data)}
{}

/*!
 * \brief Copy assignment operator.
 */
Error &Error::operator=(const Error &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

/*!
 * \brief Move assignment operator.
 */
Error &Error::operator=(Error &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

/*!
 * \brief Destructor.
 */
Error::~Error()
{
}

/*!
 * \brief Returns a human-readable error message.
 */
QString Error::text() const
{
    QStringList parts;

    if (!data->info.isEmpty())
        parts.append(data->info);

    if (!data->text.isEmpty())
        parts.append(data->text);

    if (data->sqlError.isValid())
        parts.append(data->sqlError.text());

    return parts.join("\n");
}

/*!
 * \brief Returns the underlying QSqlError.
 */
QSqlError Error::sqlError() const
{
    return data->sqlError;
}

/*!
 * \brief Returns the error type.
 */
Error::ErrorType Error::type() const
{
    return data->type;
}

/*!
 * \brief Creates an Error object from a QSqlQuery results.
 */
Error Error::fromSqlQuery(const QSqlQuery &query)
{
    const QSqlError sqlError = query.lastError();
    if (sqlError.type() != QSqlError::NoError)
        return fromSqlError("query: " + query.executedQuery(), sqlError);

    if (query.isSelect() && query.size() < 1)
        return Error(NotFoundError);

    return Error();
}

Error Error::fromSqlError(const QSqlError &error)
{
    return fromSqlError(QString(), error);
}

/*!
 * \brief Creates an Error object from a QSqlError.
 */
Error Error::fromSqlError(const QString &info, const QSqlError &error)
{
    ErrorData *data = new ErrorData();
    data->info = info;

    if (error.type() != QSqlError::NoError) {
        data->type = DatabaseError;
        data->sqlError = error;
        return Error(data);
    }

    data->type = NoError;
    return Error(data);
}

} // namespace QEloquent


QDebug operator<<(QDebug debug, const QEloquent::Error &error)
{
    const QSqlError sqlError = error.sqlError();

    debug.noquote().nospace();
    debug << "QEloquent::Error(" << error.type() << ", " << error.text();
    if (sqlError.isValid())
        debug << ", " << sqlError;
    debug << ')';

    return debug.resetFormat();
}

