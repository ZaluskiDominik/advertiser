#ifndef PRICELISTWIDGET_H
#define PRICELISTWIDGET_H

#include <QWidget>
#include "ui_pricelistwidget.h"
#include "../shared/requestreceiver.h"
#include "../shared/pricelist.h"

class PriceListTab;

class PriceListWidget : public QWidget, public RequestReceiver
{
    Q_OBJECT

public:
    //constructor for getting active price list
    explicit PriceListWidget(bool _readOnly, QWidget *parent = nullptr);
    //constructor for reading data from existing PrceList object
    explicit PriceListWidget(const PriceList& prices, bool _readOnly, QWidget *parent = nullptr);

    //fill table's cells with currently active price list
    void getActivePriceList();

    void setAssociatedTab(PriceListTab* tab);

    //requests from server change of active price list to this one
    void setActive();

protected:
    void onDataReceived(Request req, SocketHandler*) final;
    void registerRequestsReceiver(SocketHandler* socketHandler) final;

signals:
    //signal emitted when response from server to change active price list request is received
    void changeActiveResponse(PriceListTab* senderTab, bool success);

private:
    Ui::PriceListWidget ui;

    //id of this price list
    quint32 priceListId;

    //whether price list table can be modified
    bool readOnly;

    //pointer to tab associated to this price list
    PriceListTab* associatedTab;

    //sets columns' width and makes table read only if readOnly var is true
    void initTable();

    //requests from server info about currently active price list
    void sendGetActivePriceListRequest();

    //fills table with data from PriceList object
    void convertPriceListToTableContent(const PriceList& prices);

    //response to requesting active price list
    void onGetActivePriceListResponse(Request& req);
};

#endif // PRICELISTWIDGET_H
