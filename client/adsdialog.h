#ifndef ADSDIALOG_H
#define ADSDIALOG_H

#include <QDialog>
#include "ui_adsdialog.h"

class AdsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdsDialog(QWidget *parent = nullptr);

private:
    Ui::AdsDialog ui;
};

#endif // ADSDIALOG_H
