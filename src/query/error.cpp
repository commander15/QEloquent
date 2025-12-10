#include "error.h"

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

class ErrorData : public QSharedData
{
public:
    QString text;
    QSqlError sqlError;
    Error::ErrorType type = Error::NoError;
};

Error::Error()
    : data(new ErrorData)
{
}

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

Error::Error(const Error &rhs)
    : data{rhs.data}
{}

Error::Error(Error &&rhs)
    : data{std::move(rhs.data)}
{}

Error &Error::operator=(const Error &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

Error &Error::operator=(Error &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

Error::~Error()
{
}

QString Error::text() const
{
    return (data->text.isEmpty() ? data->sqlError.text() : data->text);
}

QSqlError Error::sqlError() const
{
    return data->sqlError;
}

Error::ErrorType Error::type() const
{
    return data->type;
}

Error Error::fromSqlQuery(const QSqlQuery &query)
{
    const QSqlError sqlError = query.lastError();
    if (sqlError.type() != QSqlError::NoError)
        return fromSqlError(sqlError);

    if (query.isSelect() && query.size() < 1)
        return Error(NotFoundError);

    return Error();
}

Error Error::fromSqlError(const QSqlError &error)
{
    ErrorData *data = new ErrorData();

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

