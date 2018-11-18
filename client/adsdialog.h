#ifndef ADSDIALOG_H
#define ADSDIALOG_H

#include <QDialog>
#include "ui_adsdialog.h"
#include <QVector>
#include "../shared/requestreceiver.h"

class AdsDialog : public QDialog, public RequestReceiver
{
    Q_OBJECT

public:
    explicit AdsDialog(QWidget *parent = nullptr);

protected:
    void onDataReceived(Request req, SocketHandler*) final;

private:
    Ui::AdsDialog ui;

    QVector<int> getCheckedDaysNr();

    QVector<AdsContainer*> getFilteredAdsContainers(const QVector<int> &weekDayNumbers);

    void sendAddNewAdRequest(const AdInfo& adInfo);
    void onAddNewAdResponse(Request& req);

private slots:
    //cost of user's ads changed
    void onUserAdsCostChanged(double newCost);

    //start auto picking ads btn clicked
    void on_startAutoPickingBtn_clicked();
};

#endif // ADSDIALOG_H
