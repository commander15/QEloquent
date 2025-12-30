#ifndef QELOQUENT_ITEMPROXY_H
#define QELOQUENT_ITEMPROXY_H

#include <QEloquent/global.h>

namespace QEloquent {

template<typename T>
class AbstractItemProxy
{
public:
    virtual ~AbstractItemProxy() = default;

    const T &operator*() const { return *constItem(); }
    T &operator*() { return *mutableItem(); }

    const T *operator->() const { return constItem(); }
    T *operator->() { return mutableItem(); }

    operator const T&() const { return *constItem(); }
    operator T&() { return *mutableItem(); }

protected:
    virtual const T *constItem() const = 0;
    virtual T *mutableItem() = 0;
};

template<typename T>
class ItemProxy : public AbstractItemProxy<T>
{
public:
    const T *constItem() const { return &m_proxy; }
    T *mutableItem() { return &m_proxy; }

private:
    T m_proxy;
};

} // namespace QEloquent

#endif // QELOQUENT_ITEMPROXY_H
