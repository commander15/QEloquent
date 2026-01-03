#ifndef QELOQUENT_LISTPROXY_H
#define QELOQUENT_LISTPROXY_H

#include <QEloquent/global.h>

#include <QList>

namespace QEloquent {

template<typename T>
class AbstractListProxy
{
public:
    virtual ~AbstractListProxy() = default;

    using value_type = T; // For MOC
    using iterator = QList<T>::iterator;
    using const_iterator = QList<T>::const_iterator;

    /** @brief Returns an iterator to the beginning */
    iterator begin() { return mutableList()->begin(); }
    /** @brief Returns an iterator to the end */
    iterator end() { return mutableList()->end(); }
    /** @brief Returns a const iterator to the beginning */
    const_iterator begin() const { return constList()->begin(); }
    /** @brief Returns a const iterator to the end */
    const_iterator end() const { return constList()->end(); }
    /** @brief Returns a const iterator to the beginning */
    const_iterator cbegin() const { return constList()->cbegin(); }
    /** @brief Returns a const iterator to the end */
    const_iterator cend() const { return constList()->cend(); }

    /** @brief Returns the number of related Ts */
    int count() const { return constList()->count(); }
    /** @brief Returns the number of related Ts */
    int size() const { return count(); }
    /** @brief Returns true if no related Ts exist */
    bool isEmpty() const { return count() == 0; }

    /** @brief Access a related T at a given index */
    const T &at(int i) const { return constList()->at(i); }
    /** @brief Access a related T at a given index */
    const T &operator[](int i) const { return at(i); }

    /** @brief Returns the first related T or a default instance if none found */
    T first() const { return count() > 0 ? at(0) : T(); }
    /** @brief Returns the last related T or a default instance if none found */
    T last() const { return count() > 0 ? at(count() - 1) : T(); }

protected:
    virtual const QList<T> *constList() const = 0;
    virtual QList<T> *mutableList() = 0;
};

template<typename T>
class ListProxy : public AbstractListProxy<T>
{
public:
    const QList<T> *constList() const override { return &m_proxy; }
    QList<T> *mutableList() override { return &m_proxy; }

private:
    QList<T> m_proxy;
};

} // namespace QEloquent

#endif // QELOQUENT_LISTPROXY_H
