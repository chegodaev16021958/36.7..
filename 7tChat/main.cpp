#include <QApplication>
#include "mainwindow.h"
#include "loginwindow.h"
#include "registerwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    LoginWindow* lw = new LoginWindow;
    //lw->show();

    RegisterWindow* rw = new RegisterWindow;
    //rw->show();

    QObject::connect(lw, SIGNAL(pressedRegisterButton()), rw, SLOT(show()));
    QObject::connect(rw, SIGNAL(pressedLoginButton()), lw, SLOT(show()));

    QObject::connect(lw, SIGNAL(loginAttempt(QString*,QByteArray*)), &w, SLOT(tryToLogin(QString*,QByteArray*)));
    QObject::connect(rw, SIGNAL(createNewUserAttempt(QString*,QByteArray*)), &w, SLOT(tryToCreateNewUser(QString*,QByteArray*)));

    QObject::connect(&w, SIGNAL(pressedLoginExitButton()), lw, SLOT(show()));

    return a.exec();
}
