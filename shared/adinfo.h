#ifndef ADINFO_H
#define ADINFO_H

#include <QObject>
#include "time.h"

class AdInfo
{
public:
    quint32 adId;
    quint32 ownerId;
    Time startHour;
    Time endHour;
    quint32 weekDayNr;

    //returns ad's duration in secods
    int getDuration() const;
};

QDataStream& operator>>(QDataStream& ss, AdInfo& ad);
QDataStream& operator<<(QDataStream& ss, const AdInfo& ad);

#endif // ADINFO_H
