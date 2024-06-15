#include "server.h"

Server::Server(int nPort)
{
    if(this->listen(QHostAddress::Any, nPort))
    {
        qDebug() << "server launched";
    }
    else
    {
        qDebug() << "error to start server";
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./seventchat.db");
    if (db.open()) qDebug() << "database opened";
    else qDebug() << "database didn't open";

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE Users(ID INTEGER PRIMARY KEY AUTOINCREMENT, Login TEXT, Password BLOB);");
    query->exec("CREATE TABLE Messages(ID INTEGER PRIMARY KEY AUTOINCREMENT, Sender TEXT, Receiver TEXT, Message TEXT, MessageTime BLOB);");

    modelUsers = new QSqlTableModel(this, db);
    modelUsers->setTable("Users");
    modelUsers->select();

    modelMessages = new QSqlTableModel(this, db);
    modelMessages->setTable("Messages");
    modelMessages->select();

    tempDateTime = new QDateTime;
    listUsers = new QVector<QString>;

    connect(this, SIGNAL(userLogged()), this, SLOT(sendAllPreviosMessages()));
}

void Server::sendToClient(quint8 type)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << type;
    if (type == 'M')
    {
        out << senderName << reciever << message << *tempDateTime;
        qDebug() << "sending message";
    }
    else if (type == 'S')
    {
        out << *listUsers;
    }
    socket->write(data);
}

// при подключении клиента
void Server::incomingConnection(qintptr socketDescriptor)
{
    // создаем новый сокет
    socket = new QTcpSocket;
    // устанавливаем ему дескриптор
    socket->setSocketDescriptor(socketDescriptor);
    // соединяем сигнал о готовности данных от сокета со слотом slotReadyRead
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    // соединяем сигнал о разъединении со слотом deleteLater
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    // записываем указатель на сокет в контейнер
    //socketsList.push_back(socket);
    socketsMap.insert(socket, "unknown");
    qDebug() << "client ( descriptor =" << socketDescriptor << ") connected";
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_5);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "reading";
        in >> messageType;
        switch (messageType)
        {
        case 'M':
            // новое сообщение от клиента
            in >> reciever >> message;
            *tempDateTime = QDateTime::currentDateTime();
            qDebug() << tempDateTime->toString("dd.MМ hh:mm") << "new message from " << socketsMap[socket] << " to " << reciever << ": " << message;

            senderName = socketsMap[socket];
            modelMessages->insertRow(0);
            modelMessages->setData(modelMessages->index(0, 1), senderName);
            modelMessages->setData(modelMessages->index(0, 2), reciever);
            modelMessages->setData(modelMessages->index(0, 3), message);
            modelMessages->setData(modelMessages->index(0, 4), *tempDateTime);
            if (!modelMessages->submitAll()) qDebug() << "insertion error";

            for (auto it = socketsMap.begin(); it != socketsMap.end(); ++it)
            {
                socket = it.key();
                sendToClient('M');
            }

            break;
        case 'L':
            // запрос на логин/пароль
            in >> login >> hashpassword;
            qDebug() << "login attempt:" << login << hashpassword.toHex();
            modelUsers->setFilter("Login = '" + login + "'");
            modelUsers->select();
            if (modelUsers->rowCount())
            {
                QModelIndex mIndex = modelUsers->index(0, 1);
                QVariant tempData = modelUsers->data(mIndex);
                qDebug() << tempData.toString();

                mIndex = modelUsers->index(0, 2);
                tempData = modelUsers->data(mIndex);
                QByteArray tempHash = tempData.toByteArray();
                qDebug() << tempHash.toHex();

                if(tempHash == hashpassword)
                {
                    // если хеши паролей совпадают - отвечаем 'S'
                    socketsMap[socket] = login;
                    listUsers->clear();
                    modelUsers->setFilter("");
                    modelUsers->select();
                    for (int i = 0; i < modelUsers->rowCount(); ++i)
                    {
                        QModelIndex mIndex = modelUsers->index(i, 1);
                        QVariant tempData = modelUsers->data(mIndex);
                        listUsers->push_back(tempData.toString());
                    }
//                    for (int i = 0; i < listUsers->size(); ++i)
//                    {
//                        qDebug() << (*listUsers)[i];
//                    }
                    sendToClient('S');                    
                    qDebug() << "passwords are same";
                    emit userLogged();
                }
                else
                {
                    // если хеши паролей не совпадают - отвечаем 'F'
                    sendToClient('F');
                    qDebug() << "passwords are different";
                }
            }
            else
            {
                // если такого логина в базе нет - отвечаем 'D'
                sendToClient('D');
                qDebug() << "passwords are different";
            }
            break;
        case 'N':
            // запрос на новый аккаунт
            in >> login >> hashpassword;
            qDebug() << "new login attempt:" << login << hashpassword.toHex();
            modelUsers->setFilter("Login = '" + login + "'");
            modelUsers->select();
            //qDebug() << modelUsers->rowCount();
            if (modelUsers->rowCount())
            {
                // если такой логин занят, отвечаем 'E'
                qDebug() << "login exists yet";                
                sendToClient('E');
            }
            else
            {
                // если такой логин свободен, заносим логин/паоль в базу и отвечаем 'U'
                modelUsers->insertRow(0);
                modelUsers->setData(modelUsers->index(0, 1), login);
                modelUsers->setData(modelUsers->index(0, 2), hashpassword);
                if (!modelUsers->submitAll()) qDebug() << "insertion error";
                else qDebug() << "login added";                
                sendToClient('U');
            }
            break;
        default:
            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::clientDisconnected()
{
    socket = (QTcpSocket*)sender();
    qDebug() << "client, login " << socketsMap[socket] << ", disconnected";
    socketsMap.remove(socket);
}

void Server::sendAllPreviosMessages()
{
    modelMessages->setFilter("");
    modelMessages->select();
    qDebug() << modelMessages->rowCount();
    for (int i = 0; i < modelMessages->rowCount(); ++i)
    {
        QModelIndex mIndex = modelMessages->index(i, 1);
        QVariant tempData = modelMessages->data(mIndex);
        senderName = tempData.toString();        

        mIndex = modelMessages->index(i, 2);
        tempData = modelMessages->data(mIndex);
        reciever = tempData.toString();

        mIndex = modelMessages->index(i, 3);
        tempData = modelMessages->data(mIndex);
        message = tempData.toString();

        mIndex = modelMessages->index(i, 4);
        tempData = modelMessages->data(mIndex);
        *tempDateTime = tempData.toDateTime();
        qDebug() << senderName << reciever << message << tempDateTime->toString("dd.MМ hh:mm");

        QEventLoop loop;
        QTimer::singleShot(10, &loop, SLOT(quit()));
        loop.exec();

        sendToClient('M');
    }
}
