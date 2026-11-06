#include "datatype.h"

namespace QEloquent {

void AbstractDataType::unsetValue()
{
    setValue(QVariant());
}

} // namespace QEloquent
