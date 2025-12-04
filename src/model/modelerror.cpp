#include "modelerror.h"

#include <QSqlQuery>
#include <QSqlError>

namespace QEloquent {

class ModelErrorData : public QSharedData
{
public:
    QString text;
    QSqlError sqlError;
    ModelError::ErrorType type = ModelError::NoError;
};

ModelError::ModelError()
    : data(new ModelErrorData)
{
}

ModelError::ModelError(ErrorType error, const QString &text, const QSqlError &sqlError)
    : data(new ModelErrorData)
{
    data->type = error;
    data->text = text;
    data->sqlError = sqlError;
}

ModelError::ModelError(ModelErrorData *data)
    : data(data)
{
}

ModelError::ModelError(const ModelError &rhs)
    : data{rhs.data}
{}

ModelError::ModelError(ModelError &&rhs)
    : data{std::move(rhs.data)}
{}

ModelError &ModelError::operator=(const ModelError &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

ModelError &ModelError::operator=(ModelError &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

ModelError::~ModelError()
{
}

QString ModelError::text() const
{
    return (data->text.isEmpty() ? data->sqlError.text() : data->text);
}

QSqlError ModelError::sqlError() const
{
    return data->sqlError;
}

ModelError::ErrorType ModelError::type() const
{
    return data->type;
}

ModelError ModelError::fromQuery(const QSqlQuery &query)
{
    ModelErrorData *data = new ModelErrorData();

    const QSqlError sqlError = query.lastError();
    if (sqlError.type() != QSqlError::NoError) {
        data->type = DatabaseError;
        data->sqlError = sqlError;
        return ModelError(data);
    }

    if (query.isSelect() && query.size() < 1) {
        data->type = NotFoundError;
        return ModelError(data);
    }

    data->type = NoError;
    return ModelError(data);
}

} // namespace QEloquent
