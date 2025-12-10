#ifndef QELOQUENT_DICTIONARY_H
#define QELOQUENT_DICTIONARY_H

#include <QEloquent/global.h>

namespace QEloquent {

class QELOQUENT_EXPORT Dictionary
{
public:
    static QString singular(const QString &word);
    static QString plural(const QString &word);
};

} // namespace QEloquent

#endif // QELOQUENT_DICTIONARY_H
