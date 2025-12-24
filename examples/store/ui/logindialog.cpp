#include "logindialog.h"
#include "session.h"
#include "../models/user.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

namespace Store {

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("QEloquent Store - Login");
    setFixedSize(350, 450);
    setStyleSheet("background-color: #2c3e50; color: white;");

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto logoLabel = new QLabel("Q", this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("font-size: 72px; font-weight: bold; color: #3498db;");
    layout->addWidget(logoLabel);

    auto titleLabel = new QLabel("Store Management", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #ecf0f1;");
    layout->addWidget(titleLabel);

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("Email address");
    m_emailEdit->setFixedSize(270, 40);
    m_emailEdit->setText("amadou@store.com"); // Pre-filled for convenience
    m_emailEdit->setStyleSheet("padding: 10px; border-radius: 5px; background-color: #34495e; border: none; color: white;");
    layout->addWidget(m_emailEdit);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFixedSize(270, 40);
    m_passwordEdit->setStyleSheet("padding: 10px; border-radius: 5px; background-color: #34495e; border: none; color: white;");
    layout->addWidget(m_passwordEdit);

    auto loginBtn = new QPushButton("LOGIN", this);
    loginBtn->setFixedSize(270, 45);
    loginBtn->setCursor(Qt::PointingHandCursor);
    loginBtn->setStyleSheet(
        "QPushButton { background-color: #3498db; font-weight: bold; border-radius: 5px; border: none; }"
        "QPushButton:hover { background-color: #2980b9; }"
    );
    layout->addWidget(loginBtn);

    layout->addStretch();

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::onLoginClicked()
{
    QString email = m_emailEdit->text().trimmed();
    // In this example, we simplified auth to just check email presence
    auto result = User::all(User::query().where("email", email).limit(1));
    
    if (result && !result->isEmpty()) {
        Session::instance().setUser(result->first());
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid credentials. Try amadou@store.com");
    }
}

} // namespace Store
