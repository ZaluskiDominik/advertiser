#include "adstablewidget.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>
#include <QHeaderView>
#include "../shared/time.h"
#include "../shared/userdata.h"

extern SocketHandler socketHandler;
extern UserData user;

const QColor AdsTableWidget::usersAdColor(124, 252, 0);
const QColor AdsTableWidget::otherAdColor = Qt::gray;

AdsTableWidget::AdsTableWidget(QWidget *parent)
    :QTableWidget(parent)
{
    registerRequestsReceiver(&socketHandler);

    //get active price list
    sendGetActivePriceListRequest();
}

AdsContainer *AdsTableWidget::getAdsContainer(int row, int weekDayNr)
{
    return static_cast<AdsContainer*>(cellWidget(row, weekDayNr + 1));
}

AdsContainer *AdsTableWidget::getAdsContainer(Time time, int weekDayNr)
{
    int row = time.getSeconds() / ( hoursPeriodDiff * 60 );
    return getAdsContainer(row, weekDayNr);
}

double AdsTableWidget::getUserAdsCost()
{
    return userAdsCost;
}

const PriceList &AdsTableWidget::getPriceList()
{
    return priceList;
}

void AdsTableWidget::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::GET_ACTIVE_PRICE_LIST)
        onGetActivePriceListResponse(req);
    else if(req.name == Request::GET_ADS)
        onGetAdsResponse(req);
}

void AdsTableWidget::initTable()
{
    //resize table
    setRowCount(24 * 60 / hoursPeriodDiff);
    setColumnCount(8);

    //hide vertical header
    verticalHeader()->hide();

    //set fixed height of table's cells
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(cellHeight);

    //set fixed width ot table's cells
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMinimumSectionSize(180);

    //add hours and containers for ads
    addHoursColumn();
    addsEmptyAdsContainers();
}

void AdsTableWidget::addHoursColumn()
{
    for (int i = 0, minutes = 0 ; i < rowCount() ; i++, minutes += hoursPeriodDiff)
    {
        QString hours = Time( minutes * 60 ) + " - " + Time( (minutes + hoursPeriodDiff - 1) * 60 );
        auto hoursItem = new QTableWidgetItem(hours);
        hoursItem->setTextAlignment(Qt::AlignCenter);
        hoursItem->setFont(QFont("Ubuntu", 16));
        //add item to table
        //set item's flags to read only mode
        hoursItem->setFlags( hoursItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable) );
        setItem(i, 0, hoursItem);
    }
}

void AdsTableWidget::addsEmptyAdsContainers()
{
    //set maximal total duration of ads in each hours period
    AdWidget::setMaxTotalAdsDuration( static_cast<int>(hoursPeriodDiff * 0.15) * 60 );

    //set ad's height to seconds ratio
    AdWidget::setHeightToSecondsRatio( static_cast<qreal>(cellHeight) /
                                           static_cast<qreal>(AdWidget::getMaxTotalAdsDuration()) );

    //set price list according to which AdWidget will calculate ad's cost
    AdWidget::setPriceList(&priceList);

    //set columns headers
    QStringList headers;
    headers << "Godziny" << "Poniedziałek" << "Wtorek" << "Środa" << "Czwartek" << "Piątek" << "Sobota" << "Niedziela";
    setHorizontalHeaderLabels(headers);

    //add empty containers for ads to table
    for (int i = 0 ; i < rowCount() ; i++)
    {
        QStringList timeBoundaries = item(i, 0)->text().split(" - ");
        for (int j = 1 ; j < columnCount() ; j++)
        {
            auto adContainer = new AdsContainer( timeBoundaries[0] + ":00", timeBoundaries[1] + ":59", j - 1 );
            QObject::connect(adContainer, SIGNAL(userAdsCostChanged(double)), this, SLOT(onUserAdsCostChanged(double)));
            setCellWidget(i, j, adContainer);
        }
    }
}

void AdsTableWidget::addAds(QVector<AdInfo> &ads)
{
    for (auto i = ads.begin() ; i != ads.end() ; i++)
    {
        //set appropriate background color to ad widget
        QColor adColor = ( user.id == i->ownerId ) ? usersAdColor : otherAdColor;
        int weekDayNr = static_cast<int>(i->weekDayNr);
        //add ad to ad container
        getAdsContainer(i->startHour, weekDayNr)->addAd(*i, adColor);
    }
}

void AdsTableWidget::sendGetActivePriceListRequest()
{
    socketHandler.send( Request(getReceiverId(), Request::GET_ACTIVE_PRICE_LIST) );
}

void AdsTableWidget::onGetActivePriceListResponse(Request& req)
{
    //if any errors occured
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać aktualnego cennika!");
        return;
    }

    //get price list
    QDataStream ss(&req.data, QIODevice::ReadOnly);
    ss >> priceList;

    //get ads from db
    sendGetAdsRequest();
}

void AdsTableWidget::sendGetAdsRequest()
{
    socketHandler.send( Request(getReceiverId(), Request::GET_ADS) );
}

void AdsTableWidget::onGetAdsResponse(Request &req)
{
    //if any errors occured
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać informacji o zapisanych reklamach!");
        return;
    }

    //resize table and create cells
    initTable();

    //retrieve ads and add them to table
    QVector<AdInfo> ads;
    QDataStream ss(&req.data, QIODevice::ReadOnly);
    ss >> ads;
    addAds(ads);
}

void AdsTableWidget::onUserAdsCostChanged(double change)
{
    //update cost
    userAdsCost += change;

    emit userAdsCostChanged(userAdsCost);
}
