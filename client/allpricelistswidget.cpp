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

    socketHandler->addRequestReceiver(*this);
}

void AllPriceListsWidget::sendGetAllPriceListsRequest()
{
    socketHandler.send( Request(getReceiverId(), Request::GET_ALL_PRICE_LISTS) );
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
    listWidget->setAssociatedTab(listTab);

    //connect to signals
    connectToTabsSignals(listTab);
    connectToPriceListSignals(listWidget);

    //remove addNewTab tab and space filler first
    ui.tabsLayout->removeItem(ui.spaceFiller);
    ui.tabsLayout->removeWidget(ui.addNewTab);

    //add new tab to layout
    ui.tabsLayout->addWidget(listTab);
    //set tab's name
    listTab->setText( "Cennik" + QString::number( prices.priceListId ) );
    //if price list is active, set tab's style to active and set it as current widget in stacked widget
    if (prices.isActive)
    {
       PriceListTab::activeTab = PriceListTab::selectedTab = listTab;
       listTab->selectTab();
       listTab->setActiveTab();
       ui.stackedWidget->setCurrentWidget(listWidget);
    }

    //restore addNewTab tab and space filler
    ui.tabsLayout->addWidget(ui.addNewTab);
    ui.tabsLayout->addItem(ui.spaceFiller);
}

void AllPriceListsWidget::connectToTabsSignals(PriceListTab* tab)
{
    //clicked signal
    QObject::connect(tab, SIGNAL(clicked(PriceListTab*)), this, SLOT(onTabClicked(PriceListTab*)));

    //context menu signals
    QObject::connect(tab, SIGNAL(removeClicked(PriceListWidget*)), this, SLOT(onRemovePriceListClicked(PriceListWidget*)));
    QObject::connect(tab, SIGNAL(saveChangesClicked(PriceListWidget*)), this, SLOT(onSavePriceListClicked(PriceListWidget*)));
    QObject::connect(tab, SIGNAL(setActiveClicked(PriceListWidget*)), this, SLOT(onSetActivePriceListClicked(PriceListWidget*)));
}

void AllPriceListsWidget::connectToPriceListSignals(PriceListWidget *priceList)
{
    QObject::connect(priceList, SIGNAL(changeActiveResponse(PriceListTab*, bool)), this, SLOT(onSetActiveResponse(PriceListTab*, bool)));
}

void AllPriceListsWidget::onTabClicked(PriceListTab *tab)
{
    //if it's not already selected tab, then select it
    if (tab != PriceListTab::selectedTab)
    {
        tab->selectTab();
        ui.stackedWidget->setCurrentWidget(tab->target);
    }
}

void AllPriceListsWidget::onRemovePriceListClicked(PriceListWidget *priceList)
{

}

void AllPriceListsWidget::onSavePriceListClicked(PriceListWidget *priceList)
{

}

void AllPriceListsWidget::onSetActivePriceListClicked(PriceListWidget *priceList)
{
    priceList->setActive();
}

void AllPriceListsWidget::onSetActiveResponse(PriceListTab *tab, bool success)
{
    //if any errors accured
    if ( !success )
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się zmienić aktywnego cennika!");
        return;
    }

    tab->setActiveTab();
}
