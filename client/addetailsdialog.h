#ifndef ADDETAILSDIALOG_H
#define ADDETAILSDIALOG_H

#include <QDialog>
#include "ui_addetailsdialog.h"
#include "../shared/requestreceiver.h"
#include "adwidget.h"
#include "../shared/requestreceiver.h"

class AdDetailsDialog : public QDialog, public RequestReceiver
{
    Q_OBJECT
public:
    explicit AdDetailsDialog(const AdWidget* _targetAd, QWidget *parent = nullptr);

protected:
    void onDataReceived(Request req, SocketHandler*) final;

private:
    Ui::AdDetailsDialog ui;

    const AdWidget* targetAd;

    //response from server to requesting removal of ad
    void onRemoveAdResponse(Request& req);

private slots:
    void on_removeAdBtn_clicked();

signals:
    void removedAd(AdWidget*);
};

#endif // ADDETAILSDIALOG_H
