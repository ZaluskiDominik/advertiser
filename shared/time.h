#ifndef TIME_H
#define TIME_H

#include <QString>
#include <QDataStream>

class Time
{
    friend QDataStream& operator>>(QDataStream &ss, Time& time);
    friend QDataStream& operator<<(QDataStream &ss, const Time& time);
public:
    //hh:mm:ss, hh:mm
    Time(const QString& time);
    //create from seconds
    explicit Time(int sec);
    Time() = default;

    //returns number of seconds
    int getSeconds() const;

    //returns time in format hh:mm:ss
    QString getFullHour() const;

    //returns string representation formatted to hh:mm
    operator QString() const;

    Time operator+(const Time& other) const;
    Time operator-(const Time& other) const;

    bool operator>(const Time& other) const;
    bool operator>=(const Time& other) const;
    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;

private:
    int seconds = 0;

    //change formatted time to seconds
    void calcSeconds(const QString& hour);

    //returns time in string format hh:mm or hh:mm:ss
    QString getFormattedTime(bool withSeconds = false) const;

    //adds leading zero if it's needed to: hours minutes, or seconds
    static QString addLeadingZero(const QString& timePart);
};

QDataStream& operator>>(QDataStream& ss, Time& time);
QDataStream& operator<<(QDataStream& ss, const Time& time);

#endif // TIME_H
