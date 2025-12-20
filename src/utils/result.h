#ifndef QELOQUENT_RESULT_H
#define QELOQUENT_RESULT_H

#include <expected>

namespace QEloquent {

/**
 * @brief Type alias for a result that can be either a value or an error.
 * @tparam Value The type of the value on success.
 * @tparam Error The type of the error on failure.
 */
template<typename Value, typename Error>
using Result = std::expected<Value, Error>;

/**
 * @brief Helper to create an unexpected result (error).
 * @tparam Error The error type.
 */
template<typename Error>
using unexpected = std::unexpected<Error>;

} // namespace QEloquent

#endif // QELOQUENT_RESULT_H
