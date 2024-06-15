#include <QApplication>
#include "mainwindow.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
