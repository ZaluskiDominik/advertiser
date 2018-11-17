#ifndef ADDETAILSDIALOG_H
#define ADDETAILSDIALOG_H

#include <QDialog>
#include "ui_addetailsdialog.h"
#include "../shared/requestreceiver.h"
#include "adwidget.h"

class AdDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdDetailsDialog(const AdWidget* targetAd, QWidget *parent = nullptr);

private:
    Ui::AdDetailsDialog ui;

private slots:
    void on_removeAdBtn_clicked();
};

#endif // ADDETAILSDIALOG_H
