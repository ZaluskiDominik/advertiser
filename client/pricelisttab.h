#ifndef PRICELISTTAB_H
#define PRICELISTTAB_H

#include <QLabel>
#include "pricelistwidget.h"

class PriceListTab : public QLabel
{
    Q_OBJECT
public:
    explicit PriceListTab(PriceListWidget *tabTarget, QWidget* parent = nullptr);

    //changes style of this tab to active
    void setActiveStyle();

protected:
    void mousePressEvent(QMouseEvent* e);
    void contextMenuEvent(QContextMenuEvent* e);

private:
    //widget that will be show in stacked widget when tab is clicked
    PriceListWidget* target;

    //default stylesheet applied to tab
    static QString defaultStyle;
    //style of tab which holds active price lists
    static QString activeTabStyle;
    //style of currently selected tab by user
    static QString currentTabStyle;
};

#endif // PRICELISTTAB_H
