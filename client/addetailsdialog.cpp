#include "addetailsdialog.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>
#include "../shared/userdata.h"
#include "../shared/reader.h"

extern SocketHandler socketHandler;

AdDetailsDialog::AdDetailsDialog(const AdWidget* _targetAd, QWidget *parent)
    :QDialog(parent), targetAd(_targetAd)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);

    //set startHour, duration and cost of ad in appropriate edit lines
    ui.startHour->setText( targetAd->getAdInfo().startHour.getFullHour() );
    ui.duration->setText( QString::number(targetAd->getAdInfo().getDuration()) );
    ui.cost->setText( QString::number(targetAd->getAdCost()) + "zł" );

    //request login of ad's owner
    sendGetUserDataRequest();
}

void AdDetailsDialog::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::REMOVE_AD)
        onRemoveAdResponse(req);
    else if (req.name == Request::GET_USER_DATA)
        onGetUserDataResponse(req);
}

void AdDetailsDialog::sendGetUserDataRequest()
{
    QByteArray bytes;
    QDataStream ss(&bytes, QIODevice::WriteOnly);
    ss << targetAd->getAdInfo().ownerId;

    socketHandler.send( Request(getReceiverId(), Request::GET_USER_DATA, bytes) );
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

void AdDetailsDialog::onGetUserDataResponse(Request &req)
{
    //if there wasn't any errors, set ownerLogin label's text to user's login
    if (req.status == Request::OK)
    {
        UserData userData = Reader(req).read<UserData>();

        ui.ownerLogin->setText(userData.login);
    }
}

void AdDetailsDialog::on_removeAdBtn_clicked()
{
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);
    ss << targetAd->getAdInfo().adId;

    socketHandler.send( Request(getReceiverId(), Request::REMOVE_AD, data) );
}
