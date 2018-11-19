#ifndef ADSTABLEWIDGET_H
#define ADSTABLEWIDGET_H

#include <QTableWidget>
#include "adscontainer.h"
#include "../shared/pricelist.h"
#include "../shared/requestreceiver.h"

class AdsTableWidget : public QTableWidget, public RequestReceiver
{
    Q_OBJECT
public:
    explicit AdsTableWidget(QWidget *parent = nullptr);

    //colors for ads that belongs to user and for ads that are owned by other users
    static const QColor usersAdColor;
    static const QColor otherAdColor;

    //returnw ads container object at given position in table
    AdsContainer* getAdsContainer(int row, int weekDayNr);
    AdsContainer* getAdsContainer(Time time, int weekDayNr);

    //returns cost of all ads that belongs to current user
    double getUserAdsCost();

    const PriceList& getPriceList();

protected:
    void onDataReceived(Request req, SocketHandler*) final;

private:
    //total cost of all user's ads
    double userAdsCost = 0;

    //fixed height of each cell
    const int cellHeight = 180;

    //defference between endHour and startHour in each row(in minutes)
    const int hoursPeriodDiff = 60;

    //price list according to which cost of every ad will be calculated
    PriceList priceList;

    //init table's size, cells, adds hours columns
    void initTable();
    void addHoursColumn();
    //create headers for each week day column and fill them with empty ads containers
    void addsEmptyAdsContainers();

    //add ads to table
    void addAds(const QVector<AdInfo> &ads);

    //get active price list
    void sendGetActivePriceListRequest();
    void onGetActivePriceListResponse(Request &req);

    //get ads stored in db
    void sendGetAdsRequest();
    void onGetAdsResponse(Request& req);

private slots:
    void onUserAdsCostChanged(double change);

signals:
    void userAdsCostChanged(double);
};

#endif // ADSTABLEWIDGET_H
