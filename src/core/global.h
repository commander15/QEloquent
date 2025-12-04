#ifndef QELOQUENT_GLOBAL_H
#define QELOQUENT_GLOBAL_H

#include <QtGlobal>

#include <expected>

#ifdef QELOQUENT_SHARED
#   ifdef QELOQUENT_BUILD
#       define QELOQUENT_EXPORT Q_DECL_EXPORT
#   else
#       define QELOQUENT_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define QELOQUENT_EXPORT
#endif

namespace QEloquent {

template<typename Value, typename Error>
using expected = std::expected<Value, Error>;

template<typename Error>
using unexpected = std::unexpected<Error>;

}

#endif // QELOQUENT_GLOBAL_H
