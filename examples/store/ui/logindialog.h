#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace Store {

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

private:
    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
};

} // namespace Store

#endif // LOGINDIALOG_H
