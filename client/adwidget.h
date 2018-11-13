#ifndef ADSTABLEITEM_H
#define ADSTABLEITEM_H

#include <QLabel>
#include <QColor>
#include "../shared/adinfo.h"
#include "hoverlabel.h"
#include "../shared/pricelist.h"

class AdWidget : public HoverLabel
{
    Q_OBJECT
public:
    explicit AdWidget(AdInfo& _info, QColor bcgColor, QString hoverText = "Edytuj", QWidget* parent = nullptr);

    void setAdInfo(AdInfo& _ad);

    //returns information about ad
    const AdInfo& getAdInfo();

    //returns cost of this ad
    double getAdCost();

    //static method calculating given ad's cost according to set price list
    static double calculateAdCost(Time startHour, Time endHour, int weekDayNr);

    //sets price list according to which cost of ad will be calculated
    static void setPriceList(const PriceList* list);

    //returns currently set price list
    static const PriceList& getPriceList();

    //sets maximal total duration time of all ads in one time period
    static void setMaxTotalAdsDuration(int seconds);
    static int getMaxTotalAdsDuration();

    //sets ad's height to its duration ratio
    static void setHeightToSecondsRatio(qreal height);
    static qreal getHeightToSecondsRatio();

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent*);

private:
    const static QColor hoverColor;

    //cost of this ad
    double adCost;

    //ad's information
    AdInfo info;

    //background color
    QColor color;

    //current price list
    static const PriceList* priceList;

    //max duration of ads in each hours period
    static int maxTotalAdsDuration;

    //height of add with duration 1s
    static qreal heightToSecondsRatio;

signals:
    void clicked(AdWidget*);
};

#endif // ADSTABLEITEM_H