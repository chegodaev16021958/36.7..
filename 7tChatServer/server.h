#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QVector>
#include <QMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QDateTime>
#include <QTimer>
#include <QEventLoop>

class Server : QTcpServer
{
    Q_OBJECT

public:
    Server(int nPort);
    // при каждом новом подключении создается новый сокет,
    // указатель на который записываем в контерйнер socketsList
    QTcpSocket* socket;

    QSqlTableModel* modelUsers;
    QSqlTableModel* modelMessages;

    QSqlDatabase db;

private:
    //
    QVector<QTcpSocket*> socketsList;
    QMap<QTcpSocket*,QString> socketsMap;
    QByteArray data;
    QString login;
    QString senderName;
    QString reciever;
    QString message;
    QByteArray hashpassword;
    void sendToClient(quint8 type);
    qint16 nextBlockSize = 0;
    qint8 messageType;

    QSqlQuery* query;

    QVector<QString>* listUsers;
    QDateTime* tempDateTime;

signals:
    void userLogged();

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void clientDisconnected();

private slots:
    void sendAllPreviosMessages();
};

#endif // SERVER_H
