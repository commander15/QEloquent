#ifndef MODEL_H
#define MODEL_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

template<typename Model>
using ModelHelpers = QEloquent::ModelHelpers<Model>;

template<typename Model>
using Relation = QEloquent::Relation<Model>;

class SimpleModel : public QEloquent::Model
{
    Q_GADGET
    Q_PROPERTY(int id MEMBER id)

public:
    template<typename T> SimpleModel(T *m) : QEloquent::Model(m) {}
    virtual ~SimpleModel() = default;

    int id = 0;
};

class SmartModel : public SimpleModel
{
    Q_GADGET
    Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
    Q_PROPERTY(QDateTime updatedAt MEMBER updatedAt)

public:
    template<typename T> SmartModel(T *m) : SimpleModel(m) {}
    virtual ~SmartModel() = default;

    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // MODEL_H
