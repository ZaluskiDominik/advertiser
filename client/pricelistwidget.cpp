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

PriceListTab *PriceListWidget::getAssociatedTab()
{
    return associatedTab;
}

quint32 PriceListWidget::getPriceListId()
{
    return priceListId;
}

void PriceListWidget::setActive()
{
    //serialize price list's id
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);
    ss << priceListId;

    socketHandler.send( Request(getReceiverId(), Request::CHANGE_ACTIVE_PRICE_LIST, data) );
}

void PriceListWidget::remove()
{
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);
    ss << priceListId;

    socketHandler.send( Request(getReceiverId(), Request::REMOVE_PRICE_LIST, data) );
}

void PriceListWidget::save()
{
    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);

    //convert all table fields' data to PriceList object, which will be send to server
    PriceList prices = convertTableContentToPriceList();
    ss << prices;

    socketHandler.send( Request(getReceiverId(), Request::SAVE_PRICE_LIST, data) );
}

void PriceListWidget::onDataReceived(Request req, SocketHandler *)
{   
    switch (req.name)
    {
    case Request::GET_ACTIVE_PRICE_LIST:
        onGetActivePriceListResponse(req);
        break;
    case Request::CHANGE_ACTIVE_PRICE_LIST:
        emit changeActiveResponse( associatedTab, req.status == Request::OK );
        break;
    case Request::REMOVE_PRICE_LIST:
        emit removePriceListResponse( associatedTab, req.status == Request::OK );
        break;
    case Request::SAVE_PRICE_LIST:
        emit savePriceListResponse( associatedTab, req.status == Request::OK );
        break;
    }
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
    else
    {
        //propagate table's content changed signal
        QObject::connect(ui.tableWidget, SIGNAL(cellChanged(int, int)), this, SIGNAL(priceListModified()));
    }
}

void PriceListWidget::convertPriceListToTableContent(const PriceList &prices)
{
    //set row count
    ui.tableWidget->setRowCount(prices.rows.size());

    //add rows
    for (int i = 0 ; i < prices.rows.size() ; i++)
    {
        //add hours field
        auto hours = new QTableWidgetItem(prices.rows[i].hours);
        setItemReadOnly(hours);
        ui.tableWidget->setItem(i, 0, hours);

        //add prices fields
        auto weekPrice = new QTableWidgetItem( QString::number(prices.rows[i].weekPrice));
        ui.tableWidget->setItem(i, 1, weekPrice);
        auto weekendPrice = new QTableWidgetItem( QString::number(prices.rows[i].weekendPrice));
        ui.tableWidget->setItem(i, 2, weekendPrice);

        //if table is read only make price fileds not selectable or editable
        if (readOnly)
        {
            setItemReadOnly(weekPrice);
            setItemReadOnly(weekendPrice);
        }
    }
}

PriceList PriceListWidget::convertTableContentToPriceList()
{
    PriceList list;
    list.priceListId = priceListId;

    for (int i = 0 ; i < ui.tableWidget->rowCount() ; i++)
    {
        PriceListRow row;
        row.hours = ui.tableWidget->item(i, 0)->text();
        row.weekPrice = ui.tableWidget->item(i, 1)->text().toUInt();
        row.weekendPrice = ui.tableWidget->item(i, 2)->text().toUInt();
        list.rows.append(row);
    }

    return list;
}

void PriceListWidget::sendGetActivePriceListRequest()
{
    socketHandler.send( Request(getReceiverId(), Request::GET_ACTIVE_PRICE_LIST) );
}

void PriceListWidget::setItemReadOnly(QTableWidgetItem *item)
{
    item->setFlags( item->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable) );
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
