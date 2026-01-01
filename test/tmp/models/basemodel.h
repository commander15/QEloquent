#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <QEloquent/model.h>

class BaseModel : public QEloquent::Model
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)
    Q_PROPERTY(QString createdAt MEMBER createdAt)
    Q_PROPERTY(QString updatedAt MEMBER updatedAt)

public:
    template<typename T> BaseModel(T *m) : QEloquent::Model(m) {}
    virtual ~BaseModel() = default;

    int id = 0;
    QString createdAt;
    QString updatedAt;
};

#endif // BASEMODEL_H
