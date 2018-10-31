#include "pricelist.h"

QDataStream& operator<<(QDataStream &ss, const PriceList &prices)
{
    return ss << prices.priceListId << prices.rows << prices.isActive;
}

QDataStream& operator>>(QDataStream &ss, PriceList &prices)
{
    return ss >> prices.priceListId >> prices.rows >> prices.isActive;
}

QDataStream &operator<<(QDataStream &ss, const PriceListRow &row)
{
    return ss << row.hours << row.weekPrice << row.weekendPrice;
}

QDataStream &operator>>(QDataStream &ss, PriceListRow &row)
{
    return ss >> row.hours >> row.weekPrice >> row.weekendPrice;
}
