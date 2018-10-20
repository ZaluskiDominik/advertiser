#ifndef USERSWIDGET_H
#define USERSWIDGET_H

#include <QWidget>
#include "ui_userswidget.h"

class UsersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget *parent = nullptr);
    ~UsersWidget();

private:
    Ui::UsersWidget ui;
};

#endif // USERSWIDGET_H
