#include "adinfo.h"
#include <QDataStream>

QDataStream &operator>>(QDataStream &ss, AdInfo &ad)
{
    ss >> ad.adId >> ad.endHour >> ad.ownerId  >> ad.startHour >> ad.weekDayNr;
    return ss;
}

QDataStream &operator<<(QDataStream &ss, const AdInfo &ad)
{
    ss << ad.adId << ad.endHour << ad.ownerId << ad.startHour << ad.weekDayNr;
    return ss;
}

int AdInfo::getDuration() const
{
    return (endHour - startHour).getSeconds() + 1;
}
