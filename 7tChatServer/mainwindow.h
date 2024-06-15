#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "server.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QVBoxLayout* layoutMain;
    QVBoxLayout* layoutT;
    QHBoxLayout* layoutB;

    QTableView* tableUsers;

    QPushButton* buttonDelete;
    QPushButton* buttonRefresh;
    QPushButton* buttonMessages;

    QSqlTableModel* modelU4View;
    int row = 0;
    bool flagButtonMessages = false;

public:    
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonDelete_clicked();
    void on_buttonRefresh_clicked();
    void on_buttonMessages_clicked();
    void on_tableUsers_clicked(const QModelIndex &index);

};

#endif // MAINWINDOW_H
