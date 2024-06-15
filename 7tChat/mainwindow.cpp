#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // создаем сокет
    socket = new QTcpSocket(this);

    // задаем мин размеры главного окна
    this->setMinimumHeight(480);
    this->setMinimumWidth(640);

    layoutMain = new QVBoxLayout();
    QWidget* widget = new QWidget();
    widget->setLayout(layoutMain);
    setCentralWidget(widget);

    layoutServiceBox = new QHBoxLayout();
    labelServiceText = new QLabel("Welcome to 7tChat");
    layoutServiceBox->addWidget(labelServiceText);
    buttonConnect = new QPushButton("CONNECT");
    buttonConnect->setCheckable(true);
    buttonConnect->setSizePolicy(QSizePolicy::QSizePolicy::Maximum, QSizePolicy::Maximum);
    layoutServiceBox->addWidget(buttonConnect);
    buttonLoginExit = new QPushButton("LOGIN");
    buttonLoginExit->setSizePolicy(QSizePolicy::QSizePolicy::Maximum, QSizePolicy::Maximum);
    buttonLoginExit->setDisabled(true);
    layoutServiceBox->addWidget(buttonLoginExit);
    layoutMain->addLayout(layoutServiceBox);

    textFromServer = new QTextBrowser;
    layoutMain->addWidget(textFromServer);

    layoutSendBox = new QHBoxLayout();
    buttonSend = new QPushButton("SEND");
    buttonSend->setEnabled(false);

    lineText2Send = new QLineEdit;
    comboBoxUsers = new QComboBox;
    comboBoxUsers->insertItem(1, "All users");
    comboBoxUsers->setDuplicatesEnabled(false);
    listUsers = new QVector<QString>;

    layoutSendBox->addWidget(comboBoxUsers);
    layoutSendBox->addWidget(lineText2Send);
    layoutSendBox->addWidget(buttonSend);

    layoutMain->addLayout(layoutSendBox);

    connect(buttonSend, SIGNAL(clicked(bool)), this, SLOT(sendMessageToServer()));

    connect(buttonConnect, SIGNAL(clicked(bool)), this, SLOT(tryConnect()));
    connect(socket, SIGNAL(connected()), this, SLOT(whenConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(whenDisonnected()));

    connect(buttonLoginExit, SIGNAL(clicked(bool)), this, SIGNAL(pressedLoginExitButton()));

    // соединяем сигнал о готовности данных от сокета со слотом slotReadyRead
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
}


MainWindow::~MainWindow()
{

}

void MainWindow::sendMessageToServer()
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    QString receiver = comboBoxUsers->currentText();
    out << 'M' << receiver << lineText2Send->text();
    socket->write(data);
}
// варианты исходящих к серверу:
// 'L' - запрос идентификации, логин + пароль
// 'N' - запрос регистрации, логин + пароль
// 'M' - исходящее сообщение, имя получателя, текст
// варианты входящих от сервера:
// 'S' - идентификация успешна
// 'F' - идентификация не успешна (неверный пароль)
// 'U' - новый пользователь успешно зарегистрирован
// 'R' - новый пользователь не зарегистрирован (username занят)
// 'M' - сообщение, имя отправителя, имя получателя, время, текст

void MainWindow::sendUserToServer(quint8 type, QString* user, QByteArray* hashpswd)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << type << *user << *hashpswd;
    socket->write(data);
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_5);
    if(in.status() == QDataStream::Ok)
    {
        QString str;
        quint8 type;
        in >> type;
        if (type == 'M')
        {
            //qDebug() << "new message";
            in >> senderTemp >> receiverTemp >> messageTemp >> timeTemp;
            if(receiverTemp != "All users")
            {
                str = timeTemp.toString("dd.M.yy hh:mm") + " @" + senderTemp + " to @" + receiverTemp + ":  " + messageTemp;
            }
            else
            {
                str = timeTemp.toString("dd.M.yy hh:mm") + " @" + senderTemp + " to everyone:  " + messageTemp;
            }
            textFromServer->append(str);
        }
        else if (type == 'S')
        {
            in >> *listUsers;
            comboBoxUsers->clear();
            comboBoxUsers->insertItem(1, "All users");
            for (int i = 0; i < listUsers->size(); ++i)
            {
                if((*listUsers)[i] != login)
                {
                    comboBoxUsers->insertItem((i+2),(*listUsers)[i]);
                }
            }
            str = "Connected, " + login + " logged in";
            labelServiceText->setText(str);
            connect(lineText2Send, SIGNAL(returnPressed()), this, SLOT(sendMessageToServer()));
            buttonSend->setEnabled(true);
        }
        else if (type == 'F')
        {
            str = "Connected, " + login + " didn't log in, password is wrong";
            labelServiceText->setText(str);
        }
        else if (type == 'D')
        {
            str = "Connected, " + login + " didn't log in, this username doesn't exist";
            labelServiceText->setText(str);
        }
        else if (type == 'U')
        {
            str = "Connected, user with username " + login + " was created";
            labelServiceText->setText(str);
        }
        else if (type == 'E')
        {
            str = "Connected, user with username " + login + " wasn't created, this username exists yet";
            labelServiceText->setText(str);
        }
        else qDebug() << "error, unknown message from server";
    }
    else
    {
        textFromServer->append("!error!");
        qDebug() << "!error!";
    }
}

void MainWindow::tryConnect()
{
    if (buttonConnect->isChecked() == true)
    {
        socket->connectToHost("127.0.0.1", 7077);
        //qDebug() << "connecting... ";
    }
    else
    {
        socket->disconnectFromHost();
        //qDebug() << "disconnecting... ";
    }
}

void MainWindow::whenConnected()
{
    labelServiceText->setText("Connected");
    buttonConnect->setText("DISCONNECT");
    buttonLoginExit->setDisabled(false);
    //qDebug() << "connected";
}

void MainWindow::whenDisonnected()
{
    labelServiceText->setText("Disconnected");
    buttonConnect->setText("CONNECT");
    buttonLoginExit->setDisabled(true);
    disconnect(lineText2Send, SIGNAL(returnPressed()), this, SLOT(sendMessageToServer()));
    buttonSend->setEnabled(false);
    //qDebug() << "disconnected";
}

void MainWindow::tryToLogin(QString* lgn, QByteArray* psh)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        login = *lgn;
        sendUserToServer('L', lgn, psh);
    }
}

void MainWindow::tryToCreateNewUser(QString *lgn, QByteArray *psh)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        login = *lgn;
        sendUserToServer('N', lgn, psh);
    }
}
