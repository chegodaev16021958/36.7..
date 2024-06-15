#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCryptographicHash>
#include <QValidator>

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void pressedRegisterButton();
    void loginAttempt(QString* lgn, QByteArray* psh);

private:
    QVBoxLayout* layoutMain;
    QGridLayout* layoutLabels;
    QHBoxLayout* layoutButtons;

    QLabel* labelUsername;
    QLabel* labelPassword;

    QLineEdit* lineUsername;
    QLineEdit* linePassword;

    QPushButton* buttonLogin;
    QPushButton* buttonCancel;
    QPushButton* buttonRegister;

    QString* login;
    QString* password;
    QByteArray* passwordHash;
    QRegularExpressionValidator* validUsername;

private slots:
    void buttonLoginPressed();

};

#endif // LOGINWINDOW_H
