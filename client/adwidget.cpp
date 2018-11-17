#include "adwidget.h"
#include <QPainter>
#include <cmath>

const QColor AdWidget::hoverColor(QColor(211, 0, 211, 40));

//current price list
const PriceList* AdWidget::priceList;

int AdWidget::maxTotalAdsDuration;

qreal AdWidget::heightToSecondsRatio;

AdWidget::AdWidget(AdInfo &_info, QColor bcgColor, QString hoverText, QWidget *parent)
    :HoverLabel(hoverColor, hoverText, parent)
{
    info = _info;
    color = bcgColor;
    setAlignment(Qt::AlignCenter);

    //set label with ad's time boundaries
    setText(info.startHour.getFullHour() + " - " + info.endHour.getFullHour());

    calculateCost();
}

void AdWidget::setHours(const Time &startHour, const Time &endHour)
{
    info.startHour = startHour;
    info.endHour = endHour;

    //change hours shown by label
    setText(startHour.getFullHour() + " - " + endHour.getFullHour());

    //recalculate ad's cost
    calculateCost();
}

const AdInfo& AdWidget::getAdInfo() const
{
    return info;
}

double AdWidget::getAdCost() const
{
    return adCost;
}
#include <QDebug>
double AdWidget::calculateAdCost(Time startHour, Time endHour, int weekDayNr)
{
    double cost = 0;
//    qDebug() << "hours: " << startHour.getFullHour() << "  " << endHour.getFullHour();

    QStringList priceListHours = priceList->rows[0].hours.split("-");
    int priceListHoursDiff = ( Time(priceListHours[1]) - Time(priceListHours[0]) + Time(60) ).getSeconds();
    int startIndex = startHour.getSeconds() / priceListHoursDiff;

//    qDebug() << "start index: " << startIndex;
    int i = startIndex;
    Time priceListStartHour, priceListEndHour;

    do
    {
        priceListHours = priceList->rows[i].hours.split("-");
        priceListStartHour = QString(priceListHours[0]) + ":00";
        priceListEndHour = QString(priceListHours[1]) + ":59";

        int partDuration = ( std::min(priceListEndHour, endHour) - std::max(priceListStartHour, startHour) ).getSeconds() + 1;
//        qDebug() << "end: " << std::min(priceListEndHour, endHour).getFullHour();
//        qDebug() << "start: " << std::max(priceListStartHour, startHour).getFullHour();

//        qDebug() << "part duration: " << partDuration;
        unsigned pricePerMinute = (weekDayNr >= 6) ? priceList->rows[i].weekendPrice : priceList->rows[i].weekPrice;
//        qDebug() << "per minute: " << pricePerMinute;
        cost += static_cast<double>(partDuration) * static_cast<double>(pricePerMinute) / 60.0;
        i++;
    }
    while( endHour > priceListEndHour );

    return std::round(cost * 100.0) / 100.0;
}

void AdWidget::setPriceList(const PriceList *list)
{
    priceList = list;
}

const PriceList &AdWidget::getPriceList()
{
    return *priceList;
}

void AdWidget::setMaxTotalAdsDuration(int seconds)
{
    maxTotalAdsDuration = seconds;
}

int AdWidget::getMaxTotalAdsDuration()
{
    return maxTotalAdsDuration;
}

void AdWidget::setHeightToSecondsRatio(qreal height)
{
    heightToSecondsRatio = height;
}

qreal AdWidget::getHeightToSecondsRatio()
{
    return heightToSecondsRatio;
}

void AdWidget::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    //draw ad's background
    p.setBrush(color);
    p.drawRect(0, 0, width(), height());

    HoverLabel::paintEvent(e);

    //draw bottom border
    QPainter p2(this);
    p2.setPen( QPen(Qt::black, 2) );
    p2.drawLine(0, height(), width(), height());
}

void AdWidget::mousePressEvent(QMouseEvent*)
{
    emit clicked(this);
}

void AdWidget::calculateCost()
{
    adCost = calculateAdCost(info.startHour, info.endHour, static_cast<int>(info.weekDayNr));
}
