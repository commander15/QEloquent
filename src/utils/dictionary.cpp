#include "dictionary.h"

#include <QString>

namespace QEloquent {

QString Dictionary::singular(const QString &word)
{
    if (word.endsWith("ies"))
        return word.left(word.size() - 3) + 'y';

    if (word.endsWith("s"))
        return word.left(word.size() - 1);

    return word;
}

QString Dictionary::plural(const QString &word)
{
    if (word.endsWith("y"))
        return word.left(word.size() - 1) + "ies";

    return word + "s";
}

} // namespace QEloquent
