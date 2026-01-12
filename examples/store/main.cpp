#include <QApplication>
#include <QDebug>

#include "common.h"
#include "ui/mainwindow.h"
#include "ui/logindialog.h"

#include <QEloquent/connection.h>

#include <QElapsedTimer>

using namespace QEloquent;
using namespace Store;

void initDatabase()
{
    // Configure QEloquent
    Connection::addConnection("default", "QSQLITE", "test.db");
    auto conn = Connection::defaultConnection();
    if (!conn.open()) {
        qFatal("Could not open database connection");
    }

    migrate();
    seed();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    {
        QElapsedTimer timer;
        timer.start();

        initDatabase();

        qDebug().noquote().nospace() << "initialization tooks " << timer.elapsed() << "ms";
    }

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow w;
    w.show();

    int e = a.exec();
    // rollbackMigrations();
    return e;
}
