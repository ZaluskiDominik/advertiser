#ifndef PRICELISTWIDGET_H
#define PRICELISTWIDGET_H

#include <QWidget>
#include "ui_pricelistwidget.h"
#include "../shared/requestreceiver.h"
#include "../shared/pricelist.h"

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

protected:
    void onDataReceived(Request req, SocketHandler*) final;
    void registerRequestsReceiver(SocketHandler* socketHandler) final;

private:
    Ui::PriceListWidget ui;

    //whether price list table can be modified
    bool readOnly;

    //sets columns' width and makes table read only if readOnly var is true
    void initTable();

    //fills table with data from PriceList object
    void convertPriceListToTableContent(const PriceList& prices);

    //requests currently active price list
    void sendGetActivePriceListRequest();

    //response to requesting active price list
    void onGetActivePriceListResponse(Request& req);
};

#endif // PRICELISTWIDGET_H
