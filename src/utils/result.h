#ifndef QELOQUENT_RESULT_H
#define QELOQUENT_RESULT_H

#include <expected>

namespace QEloquent {

template<typename Value, typename Error>
using Result = std::expected<Value, Error>;

template<typename Error>
using unexpected = std::unexpected<Error>;

} // namespace QEloquent

#endif // QELOQUENT_RESULT_H
