#ifndef QELOQUENT_GENERICMODELHELPERS_H
#define QELOQUENT_GENERICMODELHELPERS_H

#include <QEloquent/global.h>
#include <QEloquent/modelhelpers.h>

namespace QEloquent {

template <size_t N>
struct fixed_string {
    char data[N]{};

    // The constructor takes a reference to a C-style array
    // and copies the contents. The compiler will deduce the size N.
    constexpr fixed_string(const char (&str)[N]) {
        std::copy_n(str, N, data);
    }

    QString string() const {
        return QString::fromLatin1(data).trimmed();
    }

    operator bool() const {
        return N > 1;
    }

    // Defaulted comparison operators are required for a class NTTP
    constexpr auto operator<=>(const fixed_string&) const = default;
};


template<fixed_string table, fixed_string connection = "">
static inline GenericModel GenericModelMaker()
{ return GenericModel(); }

template<fixed_string table, fixed_string connection = "">
class GenericModelHelpers : public ModelHelpers<GenericModel, GenericModelMaker<table, connection>()>
{};

} // namespace QEloquent

#endif // QELOQUENT_GENERICMODELHELPERS_H
