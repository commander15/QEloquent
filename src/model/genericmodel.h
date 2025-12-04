#ifndef QELOQUENT_GENERICMODEL_H
#define QELOQUENT_GENERICMODEL_H

#include <QEloquent/global.h>
#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

namespace QEloquent {

class QELOQUENT_EXPORT GenericModel : public Model
{
    Q_GADGET

public:
    GenericModel();
    GenericModel(const ModelInfo &info);
    GenericModel(const GenericModel &other);
    GenericModel(GenericModel &&other);
    GenericModel &operator=(const GenericModel &other);
    GenericModel &operator=(GenericModel &&other);
    ~GenericModel();
};

template<fixed_string table, fixed_string connection = "">
class Generic : public ModelHelpers<GenericModel, table, connection>
{};

} // namespace QEloquent

#endif // QELOQUENT_GENERICMODEL_H
