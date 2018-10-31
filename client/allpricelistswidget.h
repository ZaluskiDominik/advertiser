#ifndef ALLPRICELISTSWIDGET_H
#define ALLPRICELISTSWIDGET_H

#include <QWidget>
#include "ui_allpricelistswidget.h"
#include "../shared/requestreceiver.h"
#include "../shared/pricelist.h"
#include "pricelisttab.h"

class AllPriceListsWidget : public QWidget, public RequestReceiver
{
    Q_OBJECT

public:
    explicit AllPriceListsWidget(QWidget *parent = nullptr);
    ~AllPriceListsWidget();

protected:
    void onDataReceived(Request req, SocketHandler*) final;
    void registerRequestsReceiver(SocketHandler* socketHandler) final;

private:
    Ui::AllPriceListsWidget ui;

    //gets all available price lists from server
    void sendGetAllPriceListsRequest();

    //price lists were received
    void onGetAllPriceListsResponse(Request& req);

    //adds new table with price list data to stacled widget and a tab corresponding to that price list
    void addNewPriceList(PriceList& prices);
};

#endif // ALLPRICELISTSWIDGET_H
