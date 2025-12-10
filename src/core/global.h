#ifndef QELOQUENT_GLOBAL_H
#define QELOQUENT_GLOBAL_H

#include <QtGlobal>

#ifdef QELOQUENT_SHARED
#   ifdef QELOQUENT_BUILD
#       define QELOQUENT_EXPORT Q_DECL_EXPORT
#   else
#       define QELOQUENT_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define QELOQUENT_EXPORT
#endif

#endif // QELOQUENT_GLOBAL_H
