#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include <QStackedWidget>
#include "loginwidget.h"
#include <QTcpSocket>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow ui;

    //widget displaying login form
    LoginWidget loginWidget;
    //widget displaying main application panel

    QTcpSocket s;

    //initialize stacked widget with login and main views
    void initStackedWidget();

private slots:
    //widget displayed in stacked widget was changed
    void onStackedWidgetChanged(int index);
};

#endif // MAINWINDOW_H
