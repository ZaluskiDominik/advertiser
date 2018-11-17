#ifndef ADSCONTAINER_H
#define ADSCONTAINER_H

#include "hoverlabel.h"
#include <QTableWidgetItem>
#include <QVector>
#include "adwidget.h"
#include "../shared/time.h"
#include <QVBoxLayout>
#include "adeditordialog.h"

class AdsContainer : public HoverLabel
{
    Q_OBJECT
public:
    explicit AdsContainer(Time _minTime, Time _maxTime, int _weekDayNr, QWidget* parent = nullptr);

    //adds an add
    AdWidget* addAd(AdInfo& ad, QColor adColor);

    void removeAd(AdWidget* adWidget);

    //color of added ad by user
    const static QColor userAdColor;

protected:
    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);

private:
    const static QColor defaultBcg;
    const static QColor hoverColor;

    //nr of day in week when ad will be added
    int weekDayNr;

    //vertical layout for ads
    QVBoxLayout* layout;

    //vector of added ads
    QVector<AdWidget*> ads;

    //time boundaries for ads
    const Time minTime, maxTime;

    //creates verical layout for ads and sets it to the widget
    void initLayout();

    //connects slots to signals emitted by AdEditorSignal objects
    void connectToAdEditorSignals(AdEditorDialog* adEditor);

    //calculate ad's height according to height/seconds ratio
    void calculateAdHeight(AdWidget* ad);

    //returns index where ad should be inserted according to ad's start hour
    int calculateAdIndex(const Time &adStartTime);

    //changes ad's position in container according to ad's start hour
    void repositionAd(AdWidget* ad);

    //creates and opens AdEditorDalog object, if nullptr is passed then editor will be creating new ad
    void openAdEditor(AdWidget *targetAd);

    //creates and opens AdDetailsDialog object
    void openAdDetails(AdWidget* targetAd);

private slots:
    //AdWidget was clicked
    void onAdClicked(AdWidget* adWidget);

    //user added new ad
    void onAddedNewAd(AdInfo adInfo, AdEditorDialog* adEditor);

    //ad's data was changed by user
    void onModificatedAd(AdWidget* adWidget, Time startHour, Time endHour);

    //user removed ad
    void onRemovedAd(AdWidget* adWidget);

signals:
    void userAdsCostChanged(double);
};

#endif // ADSCONTAINER_H
