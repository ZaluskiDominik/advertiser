#include "adeditordialog.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>
#include "../shared/userdata.h"
#include <QKeyEvent>

extern SocketHandler socketHandler;

extern UserData user;

AdEditorDialog::AdEditorDialog(Time _minTime, Time _maxTime, const QVector<AdWidget*>& _existingAds, int _weekDayNr, QWidget *parent)
    :QDialog(parent), existingAds(_existingAds), minTime(_minTime), maxTime(_maxTime)
{
    weekDayNr = _weekDayNr;
    init();

    //it's a new ad
    isNewAd = true;
    target = nullptr;

    //set first two numbers of start hour
    ui.startHour->setText( QString(minTime).left(2) );
    //hide button for removing ad, cause this ad isn't yet created
    ui.removeBtn->hide();
}

AdEditorDialog::AdEditorDialog(Time _minTime, Time _maxTime, const QVector<AdWidget*>& _existingAds, const AdWidget *_targetAd, QWidget *parent)
    :QDialog(parent), existingAds(_existingAds), minTime(_minTime), maxTime(_maxTime)
{
    target = _targetAd;
    weekDayNr = static_cast<int>(target->getAdInfo().weekDayNr);
    init();

    //it's an existing ad
    isNewAd = false;

    //set start hour of ad
    ui.startHour->setText(target->getAdInfo().startHour.getFullHour());
    //set ad's duration
    ui.duration->setValue(target->getAdInfo().getDuration());
    //set ad's cost
    ui.cost->setText( "Koszt: " + QString::number(target->getAdCost()) + "zł" );
}

void AdEditorDialog::setTargetAd(const AdWidget *targetAd)
{
    isNewAd = false;
    target = targetAd;
}

void AdEditorDialog::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::ADD_NEW_AD)
        onAddNewAdResponse(req);
    else if(req.name == Request::REMOVE_AD)
        onRemoveAdResponse(req);
    else if (req.name == Request::MODIFY_AD)
        onModifyAdResponse(req);
}

bool AdEditorDialog::eventFilter(QObject *target, QEvent *event)
{
    Q_UNUSED(target);
    if (event->type() == QEvent::KeyPress)
    {
        int key = static_cast<QKeyEvent*>(event)->key();
        //prevent deleting hour(first two digits) in startHour line edit
        if (key == Qt::Key_Backspace && ui.startHour->cursorPosition() < 4)
            return true;

        //prevent user from typying invalid minutet and seconds
        if ( key >= Qt::Key_6 && key <= Qt::Key_9 && !(ui.startHour->cursorPosition() % 3) )
            return true;
    }
    return QDialog::eventFilter(target, event);
}

void AdEditorDialog::init()
{
    registerRequestsReceiver(&socketHandler);
    ui.setupUi(this);
    ui.startHour->installEventFilter(this);
    initWeekDayName();
}

AdInfo AdEditorDialog::convertToAdInfo()
{
    AdInfo info;
    //start and end hours
    info.startHour = ui.startHour->text();
    info.endHour = info.startHour + Time( ui.duration->text().toInt() - 1 );
    //day's nr in week
    info.weekDayNr = static_cast<quint32>(weekDayNr);

    //add ad's id if it's existing ad
    if ( !isNewAd )
        info.adId = target->getAdInfo().adId;

    return info;
}

void AdEditorDialog::validateAd()
{
    //clear any error messages
    ui.errorLabel->clear();

    //if start hours wasn't given
    if (ui.startHour->text().size() != 8)
        ui.errorLabel->setText("Nie podano godziny rozpoczęcia reklamy");

    int duration = ui.duration->text().toInt();
    Time startHour(ui.startHour->text());
    //calculate end hour of this ad
    Time endHour = startHour + Time(duration - 1);

    //if end hour exceeds end hour boundary
    if (endHour > maxTime)
        ui.errorLabel->setText("Reklama musi się zakończyć przed " + maxTime.getFullHour());
    else
    {
        int timeLeft = freeTimeForAd();
        timeLeft -= duration;
        if ( !isNewAd )
            timeLeft += target->getAdInfo().getDuration();

        //if ad exceeds available left time
        if ( timeLeft < 0 )
        {
            ui.errorLabel->setText("Przekroczono dostępny czas o " + QString::number( -timeLeft ) + "s" );
        }
        //check if ad's time boundaries won't intersect with other ads time boundaries
        else if ( adIntersectWithExistingAds(startHour, endHour) )
        {
             ui.errorLabel->setText("O podanym czasie odtwarzana jest już inna reklama");
        }
    }
}

void AdEditorDialog::updateAdCost()
{
    Time startTime(ui.startHour->text());
    Time endTime = startTime + Time( ui.duration->text().toInt() - 1 );
    double cost = AdWidget::calculateAdCost(startTime, endTime, weekDayNr);
    ui.cost->setText("Koszt: " + QString::number(cost) + "zł");
}

void AdEditorDialog::initWeekDayName()
{
    QVector<QString> names{"Poniedziałek", "Wtorek", "Środa", "Czwartek", "Piątek", "Sobota", "Niedziela"};
    ui.weekName->setText(names[weekDayNr]);
}

bool AdEditorDialog::adIntersectWithExistingAds(const Time& startHour, const Time& endHour)
{
    for (auto i = existingAds.begin() ; i != existingAds.end() ; i++)
    {
        //if iterated ad is target ad of this ad editor, go to next iteration
        if ( (*i) == target )
            continue;

        const Time& existStartHour = (*i)->getAdInfo().startHour;
        const Time& existEndHour = (*i)->getAdInfo().endHour;
        //if intersects
        if ( ( startHour >= existStartHour && startHour <= existEndHour ) ||
             ( endHour >= existStartHour && endHour <= existEndHour ) )
                return true;
    }
    return false;
}

int AdEditorDialog::freeTimeForAd()
{
    //calculate sum of durations of all existing ads
    int takenTime = 0;
    for (auto i = existingAds.begin() ; i != existingAds.end() ; i++)
        takenTime += (*i)->getAdInfo().getDuration();

    return AdWidget::getMaxTotalAdsDuration() - takenTime;
}

void AdEditorDialog::on_saveBtn_clicked()
{
    validateAd();
    //if validation passed
    if (ui.errorLabel->text() == "")
    {
        ui.saveBtn->setDisabled(true);

        QByteArray data;
        QDataStream ss(&data, QIODevice::WriteOnly);

        //owner's id
        ss << user.id;
        //price list id
        ss << AdWidget::getPriceList().priceListId;
        //ad info
        ss << convertToAdInfo();

        //send add new ad or modify ad request depending on whether it's a new ad or existing one
        Request::RequestName reqName = ( isNewAd ) ? Request::ADD_NEW_AD : Request::MODIFY_AD;
        socketHandler.send( Request(getReceiverId(), reqName, data) );
    }
}

void AdEditorDialog::on_removeBtn_clicked()
{
    if (target == nullptr)
        return;

    int selectedBtn = QMessageBox::question(this, "Usunąć reklamę?",
        "Czy na pewno chcesz usunąć wybraną reklamę?", "Tak", "Nie", QString(), 1);

    if ( !selectedBtn )
    {
        //yes was clicked
        QByteArray data;
        QDataStream ss(&data, QIODevice::WriteOnly);
        ss << target->getAdInfo().adId;

        socketHandler.send( Request(getReceiverId(), Request::REMOVE_AD, data) );
    }
}

void AdEditorDialog::onAddNewAdResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        ui.saveBtn->setEnabled(true);
        QMessageBox::warning(this, "Błąd", "Nie udało się dodać nowej reklamy!");
        return;
    }

    //retrieve ad info with ad's id
    QDataStream ss(&req.data, QIODevice::ReadOnly);
    AdInfo adInfo;
    ss >> adInfo;

    //emit signal to AdContainer object in order to add this ad to it
    emit addedNewAd(adInfo, this);

    //change editor's mode to modifying existing ad
    isNewAd = false;
    //show button for removing ad
    ui.removeBtn->show();
}

void AdEditorDialog::onModifyAdResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        ui.saveBtn->setEnabled(true);
        QMessageBox::warning(this, "Błąd", "Nie udało się zapisać zmian!");
        return;
    }

    QDataStream ss(&req.data, QIODevice::ReadOnly);
    AdInfo adInfo;
    ss >> adInfo;

    emit modificatedAd(const_cast<AdWidget*>(target), adInfo.startHour, adInfo.endHour);
}

void AdEditorDialog::onRemoveAdResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć reklamy!");
        return;
    }

    emit removedAd(const_cast<AdWidget*>(target));
    //close ad editor
    deleteLater();
}

void AdEditorDialog::on_startHour_editingFinished()
{
    validateAd();
    if (ui.errorLabel->text() == "")
        updateAdCost();
}

void AdEditorDialog::on_startHour_cursorPositionChanged(int oldPos, int newPos)
{
    Q_UNUSED(oldPos);
    if (newPos < 3)
        ui.startHour->setCursorPosition(3);
}

void AdEditorDialog::on_duration_editingFinished()
{
    validateAd();
    if (ui.errorLabel->text() == "")
        updateAdCost();
}

void AdEditorDialog::on_duration_valueChanged(const QString&)
{
    ui.saveBtn->setEnabled(true);
}

void AdEditorDialog::on_startHour_textEdited(const QString&)
{
    ui.saveBtn->setEnabled(true);
}
