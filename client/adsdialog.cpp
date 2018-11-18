#include "adsdialog.h"
#include "../shared/userdata.h"
#include "../shared/sockethandler.h"

extern UserData user;
extern SocketHandler socketHandler;

AdsDialog::AdsDialog(QWidget *parent)
    :QDialog(parent)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);

    setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowCloseButtonHint);

    QObject::connect(ui.adsTable, SIGNAL(userAdsCostChanged(double)), this, SLOT(onUserAdsCostChanged(double)));
}

void AdsDialog::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::ADD_NEW_AD)
        onAddNewAdResponse(req);
}

QVector<int> AdsDialog::getCheckedDaysNr()
{
    QVector<int> days;
    const QObjectList& checkboxes = ui.daysGroup->children();

    for (int i = 0, j = 0 ; i < checkboxes.size() ; i++)
    {
        QCheckBox* box;
        if ( ( box = dynamic_cast<QCheckBox*>(checkboxes[i]) ) )
        {
            if (box->isChecked())
                days.push_back(j);
            j++;
        }
    }

    return days;
}

QVector<AdsContainer *> AdsDialog::getFilteredAdsContainers(const QVector<int>& weekDayNumbers)
{
    QVector<AdsContainer*> adsContainers;
    for (auto nr : weekDayNumbers)
        for (int row = 0 ; row < ui.adsTable->rowCount() ; row++)
            adsContainers.push_back( ui.adsTable->getAdsContainer(row, nr) );

    return adsContainers;
}

void AdsDialog::sendAddNewAdRequest(const AdInfo &adInfo)
{
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);

    ss << ui.adsTable->getPriceList().priceListId;
    ss << adInfo;

    socketHandler.send( Request(getReceiverId(), Request::ADD_NEW_AD, data) );
}

void AdsDialog::onAddNewAdResponse(Request &req)
{
    if (req.status == Request::ERROR)
        return;

    QDataStream ss(&req.data, QIODevice::ReadOnly);
    AdInfo adInfo;
    ss >> adInfo;

    //add new ad
    ui.adsTable->getAdsContainer(adInfo.startHour, static_cast<int>(adInfo.weekDayNr))
            ->addAd(adInfo, AdsTableWidget::usersAdColor);
}

void AdsDialog::onUserAdsCostChanged(double newCost)
{
    ui.userAdsCost->setText("Koszt reklam: " + QString::number(newCost) + "zł");
}

void AdsDialog::on_startAutoPickingBtn_clicked()
{
    QVector<AdsContainer*> adsContainers = getFilteredAdsContainers( getCheckedDaysNr() );

    auto sortFun = ( ui.cheapHours->isChecked() ) ?
        [](AdsContainer* a, AdsContainer* b) { return b->getAvgPricePerMinute() > a->getAvgPricePerMinute(); }
        :
        [](AdsContainer* a, AdsContainer* b) { return b->getAvgPricePerMinute() < a->getAvgPricePerMinute(); };

    std::sort(adsContainers.begin(), adsContainers.end(), sortFun);

    int adDuration = ui.duration->value();
    double totalCost = ui.adsTable->getUserAdsCost();

    for (int i = 0 ; i < adsContainers.size() ; i++)
    {
        bool found;
        Time startHour = adsContainers[i]->findFirstNotUsedSlot(adDuration, &found);

        //if position for ad was found
        if (found)
        {
            Time endHour = startHour + Time(adDuration) - Time(1);
            double newAdCost = AdWidget::calculateAdCost(startHour, endHour, adsContainers[i]->getWeekDayNr());

            //if current cost of ads + cost of new ad doesn't exceed the budget
            if ( newAdCost + totalCost <= ui.budget->value() )
            {
                //prepare AdInfo object
                AdInfo adInfo;
                adInfo.startHour = startHour;
                adInfo.endHour = endHour;
                adInfo.ownerId = user.id;
                adInfo.weekDayNr = static_cast<quint32>(adsContainers[i]->getWeekDayNr());

                //increase total cost of ads
                totalCost += newAdCost;

                //request from server adding new ad
                sendAddNewAdRequest(adInfo);
            }
        }
        else
        {
            //couldn't add ad to currently iterated AdsContainer object, don't consider that AdContainer futher
            adsContainers.removeAt(i--);
        }
    }
}
