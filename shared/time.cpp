#include "time.h"
#include <QStringList>

QDataStream &operator>>(QDataStream& ss, Time &time)
{
    return ss >> time.seconds;
}

QDataStream &operator<<(QDataStream& ss, const Time &time)
{
    return ss << time.seconds;
}

Time::Time(const QString &time)
{
    calcSeconds(time);
}

Time::Time(int sec)
{
    seconds = sec;
}

int Time::getSeconds() const
{
    return seconds;
}

QString Time::getFullHour() const
{
    return getFormattedTime(true);
}

Time Time::operator+(const Time &other) const
{
    return Time(seconds + other.seconds);
}

Time Time::operator-(const Time &other) const
{
    //make result none negative
    return Time( abs(seconds - other.seconds) );
}

bool Time::operator>(const Time &other) const
{
    return seconds > other.seconds;
}

bool Time::operator>=(const Time &other) const
{
   return  seconds >= other.seconds;
}

bool Time::operator<(const Time &other) const
{
    return seconds < other.seconds;
}

bool Time::operator<=(const Time &other) const
{
    return seconds < other.seconds;
}

Time::operator QString() const
{
    return getFormattedTime();
}

void Time::calcSeconds(const QString &hour)
{
    int weights[3] = {3600, 60, 1};
    //split formatted time to hours, minutes and seconds
    QStringList hourParts = hour.split(":");

    //iterate trough them, using appropriate weights sum them up to seconds
    for (auto i = 0 ; i < hourParts.size() ; i++)
        seconds += weights[i] * hourParts[i].toInt();
}

QString Time::getFormattedTime(bool withSeconds) const
{
    int weights[3] = {3600, 60, 1};
    //how much iteration(2 or 3 depending if seconds are desired)
    const int n = 2 + withSeconds;
    int sec = seconds;
    QString formattedTime;

    for (int i = 0 ; i < n ; i++)
    {
        formattedTime += addLeadingZero( QString::number(sec / weights[i]) );

        //add separator
        if (i != n -1)
            formattedTime += ":";

        sec %= weights[i];
    }

    return formattedTime;
}

QString Time::addLeadingZero(const QString &timePart)
{
    return ( timePart.size() == 1 ) ? "0" + timePart : timePart;
}
