#include "adscontainer.h"
#include <QPainter>
#include <QMouseEvent>
#include "../shared/userdata.h"
#include "addetailsdialog.h"

const QColor AdsContainer::defaultBcg(240, 240, 240);
const QColor AdsContainer::hoverColor(0, 250, 250, 40);

const QColor AdsContainer::userAdColor = Qt::green;

extern UserData user;

AdsContainer::AdsContainer(Time _minTime, Time _maxTime, int _weekDayNr, QWidget *parent)
    :HoverLabel(hoverColor, "Dodaj", parent), minTime(_minTime), maxTime(_maxTime)
{
    weekDayNr = _weekDayNr;
    initLayout();
}

AdWidget *AdsContainer::addAd(AdInfo &ad, QColor adColor)
{
    //find index where ad will be inserted
    int adIndex = calculateAdIndex(ad.startHour);

    //insert ad to layout
    AdWidget* adWidget = new AdWidget(ad, adColor, ( user.isAdmin ) ? "Wyświetl szczegóły" : "Edytuj");
    layout->insertWidget(adIndex, adWidget);

    //connect to clicked signal
    QObject::connect(adWidget, SIGNAL(clicked(AdWidget*)), this, SLOT(onAdClicked(AdWidget*)));

    //calculate ad's height according to it's duration time
    calculateAdHeight(adWidget);

    //add ad to vector of added ads
    ads.insert(adIndex, adWidget);

    return adWidget;
}

void AdsContainer::removeAd(AdWidget *adWidget)
{
    //remove ad from ads vector and layout
    ads.removeOne(adWidget);
    layout->removeWidget(adWidget);

    //delete ad
    adWidget->deleteLater();
}

void AdsContainer::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    //draw background
    p.setBrush(defaultBcg);
    p.drawRect(0 ,0, width(), height());

    //if user is a regular user, enable hover effect to indicate that add can be added
    if ( !user.isAdmin )
        HoverLabel::paintEvent(e);
}  

void AdsContainer::mousePressEvent(QMouseEvent *e)
{
    //if regular user left clicked the widget, open editor for adding new ad
    if ( e->button() == Qt::LeftButton && !user.isAdmin )
        openAdEditor(nullptr);
}

void AdsContainer::initLayout()
{
    layout = new QVBoxLayout;
    setLayout(layout);
    layout->setMargin(0);
    layout->setSpacing(0);

    //add space filler, which will take all available space
    layout->addWidget(new QWidget);
}

void AdsContainer::connectToAdEditorSignals(AdEditorDialog* adEditor)
{
    QObject::connect(adEditor, SIGNAL(addedNewAd(AdInfo, AdEditorDialog*)), this, SLOT(onAddedNewAd(AdInfo, AdEditorDialog*)));
    QObject::connect(adEditor, SIGNAL(modificatedAd(AdWidget*, Time, Time)), this, SLOT(onModificatedAd(AdWidget*, Time, Time)));
    QObject::connect(adEditor, SIGNAL(removedAd(AdWidget*)), this, SLOT(onRemovedAd(AdWidget*)));
}

void AdsContainer::calculateAdHeight(AdWidget *ad)
{
    const AdInfo& adInfo = ad->getAdInfo();
    //set ad's height multiplying hight/seconds ration times ad's duration
    ad->setFixedHeight( static_cast<int>( AdWidget::getHeightToSecondsRatio() * adInfo.getDuration() ) );
}

int AdsContainer::calculateAdIndex(const Time& adStartTime)
{
    int i;
    for (i = 0 ; i < ads.size() && adStartTime > ads[i]->getAdInfo().startHour ; i++);
    return i;
}

void AdsContainer::repositionAd(AdWidget *ad)
{
    //remove ad from layout and ads vector
    layout->removeWidget(ad);
    ads.removeOne(ad);

    //add ad to layout and ads vector in new position
    int index = calculateAdIndex(ad->getAdInfo().startHour);
    layout->insertWidget(index, ad);
    ads.insert(index, ad);
}

void AdsContainer::openAdEditor(AdWidget* targetAd)
{
    //create ad editor
    AdEditorDialog* addAd;

    //if nullptr was passed, create ad editor for a new ad
    if (targetAd == nullptr)
        addAd = new AdEditorDialog(minTime, maxTime, ads, weekDayNr, this);
    else
        addAd = new AdEditorDialog(minTime, maxTime, ads, targetAd, this);

    addAd->setAttribute(Qt::WA_DeleteOnClose);
    addAd->show();

    //init connection to signals
    connectToAdEditorSignals(addAd);
}

void AdsContainer::openAdDetails(AdWidget *targetAd)
{
    AdDetailsDialog* adDetails = new AdDetailsDialog(targetAd, this);

    adDetails->setAttribute(Qt::WA_DeleteOnClose);
    adDetails->open();
}

void AdsContainer::onAdClicked(AdWidget *adWidget)
{
    //if user is admin, open dialog with clicked ad's details
    if ( user.isAdmin )
        openAdDetails(adWidget);
    //if user isn't an admin, open editor for editing choosen ad
    else
        openAdEditor(adWidget);
}

void AdsContainer::onAddedNewAd(AdInfo adInfo, AdEditorDialog* adEditor)
{
    //add new ad
    AdWidget* newAd = addAd(adInfo, userAdColor);
    //set new ad as a target ad of sender AdEditorDialog object
    adEditor->setTargetAd(newAd);

    //emit signal informing that update of user's ads cost is needed
    emit userAdsCostChanged(newAd->getAdCost());
}

void AdsContainer::onModificatedAd(AdWidget *adWidget, Time startHour, Time endHour)
{
    double oldAdCost = adWidget->getAdCost();

    //change adWidget's hours to this typed by user
    adWidget->setHours(startHour, endHour);
    //recalculate ad's height
    calculateAdHeight(adWidget);
    //reposition ad in ad container
    repositionAd(adWidget);

    //emit signal informing that update of user's ads cost is needed
    emit userAdsCostChanged( adWidget->getAdCost() - oldAdCost );
}

void AdsContainer::onRemovedAd(AdWidget *adWidget)
{
    //emit signal informing that update of user's ads cost is needed
    emit userAdsCostChanged( -adWidget->getAdCost() );

    //remove ad
    removeAd(adWidget);
}
