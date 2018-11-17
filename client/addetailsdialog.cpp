#include "addetailsdialog.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>

extern SocketHandler socketHandler;

AdDetailsDialog::AdDetailsDialog(const AdWidget* _targetAd, QWidget *parent)
    :QDialog(parent), targetAd(_targetAd)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);

    ui.ownerLogin->setText("");
    ui.startHour->setText( targetAd->getAdInfo().startHour.getFullHour() );
    ui.duration->setText( QString::number(targetAd->getAdInfo().getDuration()) );
    ui.cost->setText( QString::number(targetAd->getAdCost()) + "zł" );
}

void AdDetailsDialog::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::REMOVE_AD)
        onRemoveAdResponse(req);
}

void AdDetailsDialog::onRemoveAdResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć reklamy!");
        return;
    }

    emit removedAd(const_cast<AdWidget*>(targetAd));
    deleteLater();
}

void AdDetailsDialog::on_removeAdBtn_clicked()
{
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);
    ss << targetAd->getAdInfo().adId;

    socketHandler.send( Request(getReceiverId(), Request::REMOVE_AD, data) );
}
