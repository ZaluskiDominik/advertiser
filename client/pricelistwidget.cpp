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

    //set price list's id
    priceListId = prices.priceListId;

    convertPriceListToTableContent(prices);
}

void PriceListWidget::getActivePriceList()
{
    sendGetActivePriceListRequest();
}

void PriceListWidget::setAssociatedTab(PriceListTab *tab)
{
    associatedTab = tab;
}

void PriceListWidget::setActive()
{
    //serialize price list's id
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);
    ss << priceListId;

    socketHandler.send( Request(getReceiverId(), Request::CHANGE_ACTIVE_PRICE_LIST, data) );
}

void PriceListWidget::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::GET_ACTIVE_PRICE_LIST)
        onGetActivePriceListResponse(req);
    else if (req.name == Request::CHANGE_ACTIVE_PRICE_LIST)
        emit changeActiveResponse( associatedTab, req.status == Request::OK );
}

void PriceListWidget::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);

    socketHandler->addRequestReceiver(*this);
}

void PriceListWidget::initTable()
{
    //set equal width of all columns
    ui.tableWidget->setColumnCount(3);
    for (int i = 0 ; i < ui.tableWidget->columnCount() ; i++)
        ui.tableWidget->setColumnWidth(i, width() / 3);

    //set table as read only if true was passed to constructor
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
    socketHandler.send( Request(getReceiverId(), Request::GET_ACTIVE_PRICE_LIST) );
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

    //set price list's id
    priceListId = prices.priceListId;

    convertPriceListToTableContent(prices);
}
