#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "ui_loginwidget.h"

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);

private slots:
    //login button clicked
    void on_pushButton_clicked();

private:
    Ui::LoginWidget ui;
};

#endif // LOGINWIDGET_H
