#include "pricelisttab.h"

QString PriceListTab::defaultStyle = QString("padding: 6px 10px 6px 10px; background-color: #ddd;") +
        "border: 1px solid #444; border-top-left-radius: 4px; border-top-right-radius: 4px;" +
        "font-size: 14px; font-weight: bold;";

QString PriceListTab::activeTabStyle = PriceListTab::defaultStyle + "background-color: lightgreen !important;";
QString PriceListTab::currentTabStyle = PriceListTab::defaultStyle + "";

PriceListTab::PriceListTab(PriceListWidget* tabTarget, QWidget *parent)
    :QLabel(parent), target(tabTarget)
{
    setStyleSheet(defaultStyle);
}

void PriceListTab::setActiveStyle()
{
    setStyleSheet(activeTabStyle);
}

void PriceListTab::mousePressEvent(QMouseEvent *e)
{

}

void PriceListTab::contextMenuEvent(QContextMenuEvent *e)
{

}
