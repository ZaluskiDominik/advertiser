#include "addetailsdialog.h"
#include "../shared/sockethandler.h"

extern SocketHandler socketHandler;

AdDetailsDialog::AdDetailsDialog(const AdWidget *targetAd, QWidget *parent)
    :QDialog(parent)
{
    ui.setupUi(this);

    ui.ownerLogin->setText("");
    ui.startHour->setText( targetAd->getAdInfo().startHour.getFullHour() );
    ui.duration->setText( QString::number(targetAd->getAdInfo().getDuration()) );
    ui.cost->setText( QString::number(targetAd->getAdCost()) + "zł" );
}

void AdDetailsDialog::on_removeAdBtn_clicked()
{
}
