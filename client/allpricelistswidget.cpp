#include "allpricelistswidget.h"
#include "ui_allpricelistswidget.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>

extern SocketHandler socketHandler;

AllPriceListsWidget::AllPriceListsWidget(QWidget *parent)
    :QWidget(parent)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);
    sendGetAllPriceListsRequest();
}

AllPriceListsWidget::~AllPriceListsWidget()
{
}

void AllPriceListsWidget::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::GET_ALL_PRICE_LISTS)
        onGetAllPriceListsResponse(req);
}

void AllPriceListsWidget::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);

    socketHandler->addRequestReceiver(std::vector<Request::RequestName>{
        Request::GET_ALL_PRICE_LISTS}, *this);
}

void AllPriceListsWidget::sendGetAllPriceListsRequest()
{
    socketHandler.send( Request(Request::GET_ALL_PRICE_LISTS) );
}

void AllPriceListsWidget::onGetAllPriceListsResponse(Request& req)
{
    //if error occured while fetching price lists
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać informacji o dostępnych cennikach!");
        return;
    }

    //retrieve all price lists and add them to stacked widget
    QDataStream ss(&req.data, QIODevice::ReadOnly);
    QVector<PriceList> priceLists;
    ss >> priceLists;

    for (auto i = priceLists.begin() ; i != priceLists.end() ; i++)
        addNewPriceList(*i);
}

void AllPriceListsWidget::addNewPriceList(PriceList &prices)
{
    //widget with table that contains price list's data
    PriceListWidget* listWidget = new PriceListWidget(prices, false, ui.stackedWidget);
    ui.stackedWidget->addWidget(listWidget);
    //tab corresponding to this price list
    PriceListTab* listTab = new PriceListTab(listWidget, this);

    //remove addNewTab tab and space filler first
    ui.tabsLayout->removeItem(ui.spaceFiller);
    ui.tabsLayout->removeWidget(ui.addNewTab);

    //add new tab to layout
    ui.tabsLayout->addWidget(listTab);
    //set tab's name
    listTab->setText( "Cennik" + QString::number( prices.priceListId ) );
    //if price list is active, set tab's style to active and set it as default widget in stacked widget
    if (prices.isActive)
    {
       listTab->setActiveStyle();
       ui.stackedWidget->setCurrentWidget(listWidget);
    }

    //restore addNewTab tab and space filler
    ui.tabsLayout->addWidget(ui.addNewTab);
    ui.tabsLayout->addItem(ui.spaceFiller);
}
