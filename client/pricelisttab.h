#ifndef PRICELISTTAB_H
#define PRICELISTTAB_H

#include <QLabel>
#include <QAction>
#include "pricelistwidget.h"

class PriceListTab : public QLabel
{
    friend class AllPriceListsWidget;
    Q_OBJECT
public:
    explicit PriceListTab(PriceListWidget *tabTarget, QWidget* parent = nullptr);

    //changes style of this tab to active, sets appropriate tooltip
    void setActiveTab();

    //changes currently selected tab to this tab, change stylesheet
    void selectTab();

protected:
    void mousePressEvent(QMouseEvent* e);
    void contextMenuEvent(QContextMenuEvent* e);

private:
    //widget that will be show in stacked widget when tab is clicked
    PriceListWidget* target;

    //tab that holds active price list
    static PriceListTab* activeTab;
    //tab that is currently selected, price list widget of that tab is visible
    static PriceListTab* selectedTab;

    //default stylesheet applied to tab
    static QString defaultStyle;
    //style of tab which holds active price lists
    static QString activeTabStyle;
    //style of currently selected tab by user
    static QString currentTabStyle;

    //context menu's actions
    //************************************
    //change currently active price list
    QAction setActiveAction;
    //save changes done in price list's table
    QAction saveChangesAction;
    //delete price list
    QAction removePriceListAction;

    //sets text to context menu actions
    void initContextMenuActionsNames();

    //initialize context menu signals that will be fired after action is triggered
    void initSignals();

signals:
    //tab was clicked
    void clicked(PriceListTab*);

    //set price list as active in context menu was clicked
    void setActiveClicked(PriceListWidget*);
    //save changes made to price list in context menu was clicked
    void saveChangesClicked(PriceListWidget*);
    //delete price lists in context menu was clicked
    void removeClicked(PriceListWidget*);

private slots:
    //slots that will propagate context menu actions trigger signals with PriceListWidget* as an argument
    void onSetActiveActionClicked();
    void onSaveChangesActionClicked();
    void onRemovePriceListActionClicked();
};

#endif // PRICELISTTAB_H
