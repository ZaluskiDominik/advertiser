#include "pricelisttab.h"
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QMenu>

PriceListTab* PriceListTab::activeTab = nullptr;
PriceListTab* PriceListTab::selectedTab = nullptr;

QString PriceListTab::defaultStyle = QString("padding: 6px 10px 6px 10px; background-color: #ddd;") +
        "border: 1px solid #444; border-top-left-radius: 4px; border-top-right-radius: 4px;" +
        "font-size: 15px;";

QString PriceListTab::activeTabStyle = PriceListTab::defaultStyle + "background-color: lightgreen !important;";
QString PriceListTab::currentTabStyle = "border: 1px solid blue !important; font-weight: bold";

PriceListTab::PriceListTab(PriceListWidget* tabTarget, QWidget *parent)
    :QLabel(parent), target(tabTarget)
{
    setStyleSheet(defaultStyle);
    initContextMenuActionsNames();
    initSignals();
    setCursor(Qt::PointingHandCursor);
}

void PriceListTab::setActiveTab()
{
    //remove active style from previous active tab
    if (activeTab == selectedTab)
        activeTab->setStyleSheet(defaultStyle + currentTabStyle);
    else
        activeTab->setStyleSheet(defaultStyle);

    //change active tab to this tab
    activeTab = this;
    if (selectedTab == this)
        setStyleSheet(activeTabStyle + currentTabStyle);
    else
        setStyleSheet(activeTabStyle);
}

void PriceListTab::selectTab()
{
    //restore previous stylesheet of tab that was being selected till now
    //if selected tab is also active tab restore to active tab
    if (selectedTab == activeTab)
        selectedTab->setStyleSheet(activeTabStyle);
    //else restore default styleheet
    else
        selectedTab->setStyleSheet(defaultStyle);

    //set this tab as a new selected tab
    selectedTab = this;
    //if it's an active tab
    if ( activeTab == this )
        setStyleSheet(activeTabStyle + currentTabStyle);
    else
        setStyleSheet(defaultStyle + currentTabStyle);
}

void PriceListTab::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        emit clicked(this);
    else
        QLabel::mousePressEvent(e);
}

void PriceListTab::contextMenuEvent(QContextMenuEvent *e)
{
    if (e->reason() == QContextMenuEvent::Mouse)
    {
        QMenu menu(this);

        //add all actions to context menu
        menu.addAction(&saveChangesAction);
        //if it's an active tab don't add remove and setActive actions
        if (activeTab != this)
        {
            menu.addAction(&setActiveAction);
            menu.addAction(&removePriceListAction);
        }

        menu.exec(e->globalPos());
    }
}

void PriceListTab::initContextMenuActionsNames()
{
    setActiveAction.setText("Ustaw aktywny");
    saveChangesAction.setText("Zapisz");
    removePriceListAction.setText("Usuń");
}

void PriceListTab::initSignals()
{
    QObject::connect(&setActiveAction, SIGNAL(triggered()), this, SLOT(onSetActiveActionClicked()));
    QObject::connect(&saveChangesAction, SIGNAL(triggered()), this, SLOT(onSaveChangesActionClicked()));
    QObject::connect(&removePriceListAction, SIGNAL(triggered()), this, SLOT(onRemovePriceListActionClicked()));
}

void PriceListTab::onSetActiveActionClicked()
{
    setActiveClicked(target);
}

void PriceListTab::onSaveChangesActionClicked()
{
    saveChangesClicked(target);
}

void PriceListTab::onRemovePriceListActionClicked()
{
    removeClicked(target);
}
