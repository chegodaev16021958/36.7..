#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork/QTcpSocket>
#include <QDateTime>
#include <QVector>
//#include <QWidget>
//#include <QVBoxLayout>
//#include <QHBoxLayout>
//#include <QLabel>
//#include <QLineEdit>
//#include <QPushButton>
//#include <QComboBox>
//#include <QShortcut>
//#include <QMessageBox>
//#include <QTextBrowser>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QVBoxLayout* layoutMain;
    QHBoxLayout* layoutServiceBox;
    QHBoxLayout* layoutSendBox;

    QPushButton* buttonLoginExit;
    QPushButton* buttonConnect;
    QPushButton* buttonSend;

    QLineEdit* lineText2Send;

    QComboBox* comboBoxUsers;

    QVector<QString>* listUsers;

    QTextBrowser* textFromServer;

    QLabel* labelServiceText;

    QTcpSocket* socket;
    QByteArray data;
    QString senderTemp, receiverTemp, messageTemp, login;
    QDateTime timeTemp;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:    
    void sendUserToServer(quint8 type, QString* user, QByteArray* hashpswd);

signals:
    void pressedLoginExitButton();


private slots:
    void slotReadyRead();
    void tryConnect();
    void whenConnected();
    void whenDisonnected();
    void sendMessageToServer();


public slots:
    void tryToLogin(QString* lgn, QByteArray* psh);
    void tryToCreateNewUser(QString* lgn, QByteArray* psh);

};
#endif // MAINWINDOW_H
