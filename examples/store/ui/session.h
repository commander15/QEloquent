#ifndef SESSION_H
#define SESSION_H

#include "../models/user.h"
#include <QObject>

namespace Store {

class Session : public QObject
{
    Q_OBJECT
public:
    static Session& instance() {
        static Session s;
        return s;
    }

    void setUser(const User &user) { m_user = user; }
    User user() const { return m_user; }
    bool isLoggedIn() const { return m_user.id > 0; }

private:
    Session() = default;
    User m_user;
};

} // namespace Store

#endif // SESSION_H
