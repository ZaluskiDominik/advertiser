#include "adscontainer.h"
#include <QPainter>
#include <QMouseEvent>
#include "userdata.h"

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
    int i;
    for (i = 0 ; i < ads.size() && ad.startHour > ads[i]->getAdInfo().startHour ; i++);

    //insert ad to layout
    AdWidget* adWidget = new AdWidget(ad, adColor, ( user.isAdmin ) ? "Wyświetl szczegóły" : "Edytuj");
    layout->insertWidget(i, adWidget);

    //connect to clicked signal
    QObject::connect(adWidget, SIGNAL(clicked(AdWidget*)), this, SLOT(onAdClicked(AdWidget*)));

    //calculate ad's height according to it's duration time
    calculateAdHeight(adWidget);

    //add ad to vector of added ads
    ads.insert(i, adWidget);

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
    QObject::connect(adEditor, SIGNAL(removedAd(AdWidget*)), this, SLOT(onRemovedAd(AdWidget*)));
}

void AdsContainer::calculateAdHeight(AdWidget *ad)
{
    const AdInfo& adInfo = ad->getAdInfo();
    ad->setFixedHeight( static_cast<int>( AdWidget::getHeightToSecondsRatio() * adInfo.getDuration() ) );
}

void AdsContainer::openAdEditor(AdWidget* targetAd)
{
    //create ad editor
    AdEditorDialog* addAd;
    if (targetAd == nullptr)
        addAd = new AdEditorDialog(minTime, maxTime, ads, weekDayNr, this);
    else
        addAd = new AdEditorDialog(minTime, maxTime, ads, targetAd, this);

    addAd->setAttribute(Qt::WA_DeleteOnClose);
    addAd->show();

    //init connection to signals
    connectToAdEditorSignals(addAd);
}

void AdsContainer::onAdClicked(AdWidget *adWidget)
{
    //if user isn't an admin, open editor for editing choosen ad
    if ( !user.isAdmin )
        openAdEditor(adWidget);
}

void AdsContainer::onAddedNewAd(AdInfo adInfo, AdEditorDialog* adEditor)
{
    AdWidget* newAd = addAd(adInfo, userAdColor);
    //set new ad as a target ad of sender AdEditorDialog object
    adEditor->setTargetAd(newAd);
}

void AdsContainer::onRemovedAd(AdWidget *adWidget)
{
    removeAd(adWidget);
}
