#ifndef QELOQUENT_DEBUG_P_H
#define QELOQUENT_DEBUG_P_H

#include <QLoggingCategory>

#define qelDebug()    qCDebug(qeloquent).noquote().nospace()
#define qelInfo()     qCInfo(qeloquent).noquote().nospace()
#define qelWarning()  qCWarning(qeloquent).noquote().nospace()
#define qelCritical() qCCritical(qeloquent).noquote().nospace()

Q_DECLARE_LOGGING_CATEGORY(qeloquent)

#endif // QELOQUENT_DEBUG_P_H
