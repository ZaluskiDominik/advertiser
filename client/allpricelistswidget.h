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

protected:
    void onDataReceived(Request req, SocketHandler*) final;

private:
    Ui::AllPriceListsWidget ui;

    //gets all available price lists from server
    void sendGetAllPriceListsRequest();

    //price lists were received
    void onGetAllPriceListsResponse(Request& req);

    //adds new table with price list data to stacled widget and a tab corresponding to that price list
    void addPriceList(PriceList& prices);

    //connects to tab's context menu action's trigger signals and clicked signal
    void connectToTabsSignals(PriceListTab* tab);

    //connects to responses from server send by price list widget's signals
    void connectToPriceListSignals(PriceListWidget* priceList);

private slots:
    //tab was clicked, change stacked widget view to this tab's price list
    void onTabClicked(PriceListTab* tab);

    //remove a price list
    void onRemovePriceListClicked(PriceListWidget* priceList);
    //save changes made to price list
    void onSavePriceListClicked(PriceListWidget* priceList);
    //set a price list as active
    void onSetActivePriceListClicked(PriceListWidget* priceList);

    //response to remove price list request
    void onRemovePriceListResponse(PriceListTab* tab, bool success);
    //response to save price list request
    void onSavePriceListResponse(PriceListTab* tab, bool success);
    //response to requesting change of active price list
    void onSetActiveResponse(PriceListTab* tab, bool success);

    //user clicked add new tab button, adds new empty price list
    void on_addNewTab_clicked();

    //price list's content was changed
    void onPriceListModified();

    //response to requesting creation of a new price list
    void onAddNewPriceListResponse(Request& req);
};

#endif // ALLPRICELISTSWIDGET_H
