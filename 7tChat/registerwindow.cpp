#include "registerwindow.h"

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget{parent}
{
    layoutMain = new QVBoxLayout;
    layoutLabels = new QGridLayout;
    layoutButtons = new QHBoxLayout;
    labelUsername = new QLabel("Username:");
    labelUsername->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    labelPassword = new QLabel("Password:");
    labelPassword->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    labelConfirm = new QLabel("Confirm:");
    labelConfirm->setAlignment(Qt::AlignCenter | Qt::AlignRight);

    lineUsername = new QLineEdit;
    lineUsername->setPlaceholderText("type your username here...");
    QRegularExpression expr("[0-9a-zA-Z]{1,20}");
    validUsername = new QRegularExpressionValidator(expr, this);
    lineUsername->setValidator(validUsername);
    lineUsername->setMaxLength(20);

    linePassword = new QLineEdit;
    linePassword->setPlaceholderText("type your password here...");
    linePassword->setMaxLength(30);
    linePassword->setEchoMode(QLineEdit::Password);

    lineConfirm = new QLineEdit;
    lineConfirm->setPlaceholderText("confirm your password here...");
    lineConfirm->setMaxLength(30);
    lineConfirm->setEchoMode(QLineEdit::Password);

    buttonRegister = new QPushButton("Register");
    buttonLogin = new QPushButton("Login");
    buttonCancel= new  QPushButton("Cancel");

    login = new  QString;
    password = new  QString;
    confirm = new  QString;
    passwordHash = new QByteArray;

    layoutMain->addLayout(layoutLabels);
    layoutMain->addLayout(layoutButtons);

    layoutLabels->addWidget(labelUsername, 0, 0);
    layoutLabels->addWidget(lineUsername, 0, 1);
    layoutLabels->addWidget(labelPassword, 1, 0);
    layoutLabels->addWidget(linePassword, 1, 1);
    layoutLabels->addWidget(labelConfirm, 2, 0);
    layoutLabels->addWidget(lineConfirm, 2, 1);

    layoutButtons->addWidget(buttonRegister);
    layoutButtons->addWidget(buttonCancel);
    layoutButtons->addWidget(buttonLogin);

    setLayout(layoutMain);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle("New user?");
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    QObject::connect(buttonRegister, SIGNAL(clicked(bool)), this, SLOT(buttonRegisterPressed()));

    QObject::connect(buttonCancel, SIGNAL(clicked(bool)), this, SLOT(close()));

    QObject::connect(buttonLogin, SIGNAL(clicked(bool)), this, SIGNAL(pressedLoginButton()));
    QObject::connect(buttonLogin, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void RegisterWindow::buttonRegisterPressed()
{
    *login = lineUsername->text();
    *password = linePassword->text();
    *confirm = lineConfirm->text();
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
    else if(confirm->isEmpty())
    {
        lineConfirm->setPlaceholderText("CONFIRM PASSWORD");
        return;
    }
    else if(QString::compare(*password, *confirm, Qt::CaseSensitive))
    {
        lineConfirm->clear();
        lineConfirm->setPlaceholderText("NOT THE SAME PASSWORD");
        return;
    }
    else
    {
        //qDebug() << *login << ' ' << *password;
        QByteArray tempStr = password->toUtf8();
        *passwordHash = QCryptographicHash::hash(tempStr, QCryptographicHash::Sha256);
        emit createNewUserAttempt(login, passwordHash);
        this->close();
        return;
    }
}
