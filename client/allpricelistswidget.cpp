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
    else if (req.name == Request::ADD_NEW_PRICE_LIST)
        onAddNewPriceListResponse(req);
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
        addPriceList(*i);
}

void AllPriceListsWidget::addPriceList(PriceList &prices)
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
    //change active price list, remove price list and save price list responses
    QObject::connect(priceList, SIGNAL(changeActiveResponse(PriceListTab*, bool)), this, SLOT(onSetActiveResponse(PriceListTab*, bool)));
    QObject::connect(priceList, SIGNAL(removePriceListResponse(PriceListTab*, bool)), this, SLOT(onRemovePriceListResponse(PriceListTab*, bool)));
    QObject::connect(priceList, SIGNAL(savePriceListResponse(PriceListTab*, bool)), this, SLOT(onSavePriceListResponse(PriceListTab*, bool)));

    //price list modified signal
    QObject::connect(priceList, SIGNAL(priceListModified()), this, SLOT(onPriceListModified()));
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
    priceList->remove();
}

void AllPriceListsWidget::onSavePriceListClicked(PriceListWidget *priceList)
{
    //if there were any changes in price list to save
    if ( priceList->getAssociatedTab()->text().back() == '*' )
        priceList->save();
}

void AllPriceListsWidget::onSetActivePriceListClicked(PriceListWidget *priceList)
{
    priceList->setActive();
}

void AllPriceListsWidget::onRemovePriceListResponse(PriceListTab *tab, bool success)
{
    //if any errors occured
    if ( !success )
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć wybranego cennika!");
        return;
    }

    //remove tab from layout
    ui.tabsLayout->removeWidget(tab);
    //remove price list widget from stacked widget and delete it
    ui.stackedWidget->removeWidget(tab->target);
    tab->target->deleteLater();
    //delete tab
    tab->deleteLater();

    PriceListTab::selectedTab = static_cast<PriceListWidget*>(ui.stackedWidget->currentWidget())->getAssociatedTab();
    PriceListTab::selectedTab->selectTab();
}

void AllPriceListsWidget::onSavePriceListResponse(PriceListTab *tab, bool success)
{
    //if any errors occured
    if ( !success )
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się zapisać zmian w wybranym cenniku!");
        return;
    }

    //remove * from tab's name
    tab->setText("Cennik" + QString::number(tab->target->getPriceListId()));
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

void AllPriceListsWidget::on_addNewTab_clicked()
{
    PriceList list;
    list.isActive = false;
    int period = 30;

    //create list of time periods
    //prices will be set to 0
    for (int min = 0 ; min < 24 * 60 ; min += period)
    {
        PriceListRow row;
        row.hours = PriceListWidget::convertToTime(min) + "-" + PriceListWidget::convertToTime(min + period - 1);
        list.rows.append(row);
    }

    QByteArray data;
    QDataStream ss(&data, QIODevice::WriteOnly);
    ss << list;

    socketHandler.send( Request(getReceiverId(), Request::ADD_NEW_PRICE_LIST, data) );
}

void AllPriceListsWidget::onPriceListModified()
{
    PriceListTab* tab = qobject_cast<PriceListWidget*>(sender())->getAssociatedTab();
    //mark tab as modified with *
    tab->setText("Cennik" + QString::number(tab->target->getPriceListId()) + "*");
}

void AllPriceListsWidget::onAddNewPriceListResponse(Request &req)
{
    //if any errors occured
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się stworzyć nowego cennika!");
        return;
    }

    QDataStream ss(&req.data, QIODevice::ReadOnly);
    PriceList list;
    ss >> list;

    addPriceList(list);
}
