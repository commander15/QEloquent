#ifndef INVALIDMODELS_H
#define INVALIDMODELS_H

#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

class NoGadgetModel : public QEloquent::Model,
                      public QEloquent::ModelHelpers<NoGadgetModel>
{};

class NoPropertiesModel : public QEloquent::Model,
                          public QEloquent::ModelHelpers<NoPropertiesModel>
{ Q_GADGET };

class NoIdModel : public QEloquent::Model, public QEloquent::ModelHelpers<NoIdModel>
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)

public:
    QString name;
};

#endif // INVALIDMODELS_H
