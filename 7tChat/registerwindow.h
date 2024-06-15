#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCryptographicHash>
#include <QValidator>

class RegisterWindow : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWindow(QWidget *parent = nullptr);   

signals:
    void pressedLoginButton();
    void createNewUserAttempt(QString* lgn, QByteArray* psh);

private:
    QVBoxLayout* layoutMain;
    QGridLayout* layoutLabels;
    QHBoxLayout* layoutButtons;

    QLabel* labelUsername;
    QLabel* labelPassword;
    QLabel* labelConfirm;

    QLineEdit* lineUsername;
    QLineEdit* linePassword;
    QLineEdit* lineConfirm;

    QPushButton* buttonRegister;
    QPushButton* buttonCancel;
    QPushButton* buttonLogin;

    QString* login;
    QString* password;
    QString* confirm;
    QByteArray* passwordHash;
    QRegularExpressionValidator* validUsername;

private slots:
    void buttonRegisterPressed();
};

#endif // REGISTERWINDOW_H
