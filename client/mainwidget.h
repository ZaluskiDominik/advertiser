#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "ui_mainwidget.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);

private:
    Ui::MainWidget ui;
};

#endif // MAINWIDGET_H
