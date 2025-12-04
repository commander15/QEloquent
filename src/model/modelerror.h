#ifndef QELOQUENT_MODELERROR_H
#define QELOQUENT_MODELERROR_H

#include <QEloquent/global.h>

#include <QSharedDataPointer>
#include <QSqlError>

class QSqlQuery;

namespace QEloquent {

class ModelErrorData;
class QELOQUENT_EXPORT ModelError
{
public:
    enum ErrorType {
        NoError,
        NotFoundError,
        DatabaseError
    };

    ModelError();
    ModelError(ErrorType error, const QString &text = QString(), const QSqlError &sqlError = QSqlError());
    ModelError(const ModelError &);
    ModelError(ModelError &&);
    ModelError &operator=(const ModelError &);
    ModelError &operator=(ModelError &&);
    ~ModelError();

    QString text() const;
    QSqlError sqlError() const;
    ErrorType type() const;

    static ModelError fromQuery(const QSqlQuery &query);

private:
    ModelError(ModelErrorData *data);

    QSharedDataPointer<ModelErrorData> data;
};

} // namespace QEloquent

#endif // QELOQUENT_MODELERROR_H
