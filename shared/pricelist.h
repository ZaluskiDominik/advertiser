#ifndef PRICELIST_H
#define PRICELIST_H

#include <QDataStream>
#include <QVector>

struct PriceListRow
{
    QString hours;
    quint32 weekPrice = 0;
    quint32 weekendPrice = 0;
};

QDataStream& operator<<(QDataStream& ss, const PriceListRow& row);
QDataStream& operator>>(QDataStream& ss, PriceListRow& row);

struct PriceList
{   
    quint32 priceListId;
    bool isActive;
    QVector<PriceListRow> rows;
};

QDataStream& operator<<(QDataStream& ss, const PriceList& prices);
QDataStream& operator>>(QDataStream& ss, PriceList& prices);

#endif // PRICELIST_H
