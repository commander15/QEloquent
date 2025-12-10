#ifndef QELOQUENT_GENERICMODEL_H
#define QELOQUENT_GENERICMODEL_H

#include <QEloquent/global.h>
#include <QEloquent/model.h>
#include <QEloquent/modelhelpers.h>

namespace QEloquent {

class QELOQUENT_EXPORT GenericModel : public Model
{
    Q_GADGET

public:
    GenericModel();
    GenericModel(const ModelInfo &info);
    GenericModel(const GenericModel &other);
    GenericModel(GenericModel &&other);
    GenericModel &operator=(const GenericModel &other);
    GenericModel &operator=(GenericModel &&other);
    ~GenericModel();
};

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

template<fixed_string table, fixed_string connection>
class GenericModelMaker
{
public:
    static GenericModel make()
    { return GenericModel(info()); }

    static ModelInfo info()
    {
        const Connection con = (connection ? Connection::connection(connection.string()) : Connection::defaultConnection());
        return con.modelInfo(table.string());
    }
};

template<fixed_string table, fixed_string connection = "">
class Generic : public ModelHelpers<GenericModel, GenericModelMaker<table, connection>>
{};

} // namespace QEloquent

#endif // QELOQUENT_GENERICMODEL_H
