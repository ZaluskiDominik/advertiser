#include "pricelistwidget.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>

extern SocketHandler socketHandler;

PriceListWidget::PriceListWidget(bool _readOnly, QWidget *parent)
    :QWidget(parent), readOnly(_readOnly)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);
    initTable();
    sendGetActivePriceListRequest();
}

PriceListWidget::PriceListWidget(const PriceList &prices, bool _readOnly, QWidget *parent)
    :QWidget(parent), readOnly(_readOnly)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);
    initTable();

    convertPriceListToTableContent(prices);
}

void PriceListWidget::getActivePriceList()
{
    sendGetActivePriceListRequest();
}

void PriceListWidget::onDataReceived(Request req, SocketHandler *)
{
    onGetActivePriceListResponse(req);
}

void PriceListWidget::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);

    socketHandler->addRequestReceiver(std::vector<Request::RequestName>{
                                          Request::GET_ACTIVE_PRICE_LIST}, *this);
}

void PriceListWidget::initTable()
{
    //set equal width of all columns
    ui.tableWidget->setColumnCount(3);
    for (int i = 0 ; i < ui.tableWidget->columnCount() ; i++)
        ui.tableWidget->setColumnWidth(i, width() / 3);

    if (readOnly)
        ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PriceListWidget::convertPriceListToTableContent(const PriceList &prices)
{
    //set row count
    ui.tableWidget->setRowCount(prices.rows.size());

    //add rows
    for (int i = 0 ; i < prices.rows.size() ; i++)
    {
        ui.tableWidget->setItem(i, 0, new QTableWidgetItem( prices.rows[i].hours ));
        ui.tableWidget->setItem(i, 1, new QTableWidgetItem( QString::number(prices.rows[i].weekPrice)) );
        ui.tableWidget->setItem(i, 2, new QTableWidgetItem( QString::number(prices.rows[i].weekendPrice)) );
    }
}

void PriceListWidget::sendGetActivePriceListRequest()
{
    socketHandler.send( Request(Request::GET_ACTIVE_PRICE_LIST) );
}

void PriceListWidget::onGetActivePriceListResponse(Request &req)
{
    //if any error occured on server side
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać danych o aktualnym cenniku!");
        return;
    }

    QDataStream ss(&req.data, QIODevice::ReadOnly);
    PriceList prices;
    ss >> prices;

    convertPriceListToTableContent(prices);
}
