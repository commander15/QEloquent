#include "dictionary.h"

#include <QString>

namespace QEloquent {

/*!
 * \class Dictionary
 * \brief Simple utility class for English singular/plural word transformations.
 *
 * Dictionary provides basic, rule-based conversion between singular and plural forms
 * of common English nouns. It is primarily used internally by QEloquent for naming
 * conventions (e.g. converting model names like "UserRole" → "user_roles" table).
 *
 * The implementation uses very simple heuristic rules and is **not** intended to be
 * a complete linguistic tool. It covers the most frequent cases but may produce
 * incorrect results for irregular nouns (e.g. "child" → "children", "person" → "people").
 *
 * \note All methods are static and thread-safe.
 */

/*!
 * \brief Converts a plural word to its singular form using simple rules.
 *
 * Applies the following transformations:
 * - words ending in "ies" → replace with "y" (e.g. "categories" → "category")
 * - words ending in "s" → remove the "s" (e.g. "users" → "user")
 * - all other words → returned unchanged
 *
 * \param word The word (usually plural) to convert.
 * \return The guessed singular form.
 *
 * \note This is a heuristic and does not handle irregular plurals.
 */
QString Dictionary::singular(const QString &word)
{
    if (word.endsWith("ies"))
        return word.left(word.size() - 3) + 'y';
    if (word.endsWith("s"))
        return word.left(word.size() - 1);
    return word;
}

/*!
 * \brief Converts a singular word to its plural form using simple rules.
 *
 * Applies the following transformations:
 * - words ending in "y" → replace "y" with "ies" (e.g. "category" → "categories")
 * - all other words → append "s" (e.g. "user" → "users")
 *
 * \param word The word (usually singular) to pluralize.
 * \return The guessed plural form.
 *
 * \note This is a heuristic and does not handle irregular plurals.
 */
QString Dictionary::plural(const QString &word)
{
    if (word.endsWith("y"))
        return word.left(word.size() - 1) + "ies";
    return word + "s";
}

} // namespace QEloquent
