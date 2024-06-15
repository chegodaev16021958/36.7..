#include "loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget{parent}
{
    layoutMain = new QVBoxLayout;
    layoutLabels = new QGridLayout;
    layoutButtons = new QHBoxLayout;
    labelUsername = new QLabel("Username:");
    labelUsername->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    labelPassword = new QLabel("Password:");
    labelPassword->setAlignment(Qt::AlignCenter | Qt::AlignRight);

    lineUsername = new QLineEdit;
    lineUsername->setPlaceholderText("type your username here...");
    QRegularExpression expr("[0-9a-zA-Z]{1,20}");
    validUsername = new QRegularExpressionValidator(expr, this);
    lineUsername->setValidator(validUsername);
    lineUsername->setMaxLength(20);    

    linePassword = new QLineEdit;
    linePassword->setPlaceholderText("type your password here...");
    linePassword->setEchoMode(QLineEdit::Password);
    linePassword->setMaxLength(30);

    buttonLogin = new QPushButton("Login");
    buttonCancel= new  QPushButton("Cancel");
    buttonRegister = new QPushButton("Register");

    login = new  QString;
    password = new  QString;
    passwordHash = new QByteArray;

    layoutMain->addLayout(layoutLabels);
    layoutMain->addLayout(layoutButtons);

    layoutLabels->addWidget(labelUsername, 0, 0);
    layoutLabels->addWidget(lineUsername, 0, 1);
    layoutLabels->addWidget(labelPassword, 1, 0);
    layoutLabels->addWidget(linePassword, 1, 1);

    layoutButtons->addWidget(buttonLogin);
    layoutButtons->addWidget(buttonCancel);
    layoutButtons->addWidget(buttonRegister);

    setLayout(layoutMain);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle("Login to 7tChat");
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    QObject::connect(buttonLogin, SIGNAL(clicked(bool)), this, SLOT(buttonLoginPressed()));

    QObject::connect(buttonCancel, SIGNAL(clicked(bool)), this, SLOT(close()));

    QObject::connect(buttonRegister, SIGNAL(clicked(bool)), this, SIGNAL(pressedRegisterButton()));
    QObject::connect(buttonRegister, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void LoginWindow::buttonLoginPressed()
{
    *login = lineUsername->text();
    *password = linePassword->text();
    if(login->length() <= 2)
    {
        lineUsername->setPlaceholderText("USERNAME, MIN 3 SYMBOLS");
        return;
    }
    else if(password->length() <= 3)
    {
        linePassword->setPlaceholderText("PASSWORD, MIN 4 SYMBOLS");
        return;
    }
    else
    {
        //qDebug() << *login << ' ' << *password;
        QByteArray tempStr = password->toUtf8();
        *passwordHash = QCryptographicHash::hash(tempStr, QCryptographicHash::Sha256);
        emit loginAttempt(login, passwordHash);
        this->close();
        return;
    }
}
