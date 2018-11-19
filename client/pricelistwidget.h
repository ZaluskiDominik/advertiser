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

    //returns tab associated with this price list, or nullptr id there isn't one
    PriceListTab* getAssociatedTab();

    quint32 getPriceListId();

    //requests from server change of active price list to this one
    void setActive();

    //requests from server removal of this price list
    void remove();

    //requests from server savement of changes made to this price list
    void save();

protected:
    void onDataReceived(Request req, SocketHandler*) final;

signals:
    //signal emitted when response from server to change active price list request is received
    void changeActiveResponse(PriceListTab* senderTab, bool success);
    //signal emitted when response from server to remove price list request is received
    void removePriceListResponse(PriceListTab* senderTab, bool success);
    //signal emitted when response from server to save price list request is received
    void savePriceListResponse(PriceListTab* senderTab, bool success);

    //content of price list was modified
    void priceListModified();

private:
    Ui::PriceListWidget ui;

    //id of this price list
    quint32 priceListId;

    //whether price list table can be modified
    bool readOnly;

    QVector<PriceListRow> prices;

    //pointer to tab associated to this price list
    PriceListTab* associatedTab = nullptr;

    //sets columns' width and makes table read only if readOnly var is true
    void initTable();

    //requests from server info about currently active price list
    void sendGetActivePriceListRequest();

    //makes table item read only and not selectable
    void setItemReadOnly(QTableWidgetItem* item);

    //fills table with data from PriceList object
    void convertPriceListToTableContent(const PriceList& prices);

    //converts data contained within table to PriceList object
    PriceList convertTableContentToPriceList();

    //response to requesting active price list
    void onGetActivePriceListResponse(Request& req);

private slots:
    void onCellChanged(int row, int col);
};

#endif // PRICELISTWIDGET_H
