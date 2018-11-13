#include "adeditordialog.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>
#include "userdata.h"
#include <QKeyEvent>

extern SocketHandler socketHandler;

extern UserData user;

AdEditorDialog::AdEditorDialog(Time _minTime, Time _maxTime, const QVector<AdWidget*>& _existingAds, int _weekDayNr, QWidget *parent)
    :QDialog(parent), weekDayNr(_weekDayNr), existingAds(_existingAds), minTime(_minTime), maxTime(_maxTime)
{
    registerRequestsReceiver(&socketHandler);
    ui.setupUi(this);
    isNewAd = true;
    target = nullptr;

    ui.startHour->setText( QString(minTime).left(2) );
    ui.removeBtn->hide();
    ui.startHour->installEventFilter(this);

    initWeekDayName();
}

AdEditorDialog::AdEditorDialog(Time _minTime, Time _maxTime, const QVector<AdWidget*>& _existingAds, AdWidget *_targetAd, QWidget *parent)
    :QDialog(parent), existingAds(_existingAds), minTime(_minTime), maxTime(_maxTime)
{
    registerRequestsReceiver(&socketHandler);
    ui.setupUi(this);
    isNewAd = false;
    target = _targetAd;
    weekDayNr = static_cast<int>(target->getAdInfo().weekDayNr);

    ui.startHour->setText(target->getAdInfo().startHour);
    ui.startHour->installEventFilter(this);

    initWeekDayName();
}

void AdEditorDialog::setTargetAd(AdWidget *targetAd)
{
    target = targetAd;
}

void AdEditorDialog::onDataReceived(Request req, SocketHandler *)
{
    if (req.name == Request::ADD_NEW_AD)
        onAddNewAdResponse(req);
    else if(req.name == Request::REMOVE_AD)
        onRemoveAdResponse(req);
}

void AdEditorDialog::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);

    socketHandler->addRequestReceiver(*this);
}

bool AdEditorDialog::eventFilter(QObject *target, QEvent *event)
{
    Q_UNUSED(target);
    if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Backspace && ui.startHour->cursorPosition() < 4)
            return true;
    }
    return QDialog::eventFilter(target, event);
}

AdInfo AdEditorDialog::convertToAdInfo()
{
    AdInfo info;
    //start and end hours
    info.startHour = ui.startHour->text();
    info.endHour = info.startHour + Time( ui.duration->text().toInt() - 1 );

    if ( !isNewAd )
    {
        //add ad's id if it's existing ad
        info.adId = target->getAdInfo().adId;
        //add week day number retrieved from target AdWidget object
        info.weekDayNr = target->getAdInfo().weekDayNr;
    }
    else
    {
        //it's a new ad
        //add week day number passed trough constructor parameter
        info.weekDayNr = static_cast<quint32>(weekDayNr);
    }

    return info;
}

void AdEditorDialog::validateAd()
{
    //clear any error messages
    ui.errorLabel->clear();

    //if start hours wasn't given
    if (ui.startHour->text().size() != 8)
        ui.errorLabel->setText("Nie podano godziny rozpoczęcia reklamy");

    Time startHour(ui.startHour->text());
    //calculate end hour of this ad
    Time endHour = startHour + Time(ui.duration->text().toInt() - 1);

    //if end hour exceeds end hour boundary
    if (endHour > maxTime)
        ui.errorLabel->setText("Reklama musi się zakończyć przed " + maxTime.getFullHour());
    else
    {
        //calculate sum of durations of all existing ads
        int takenTime = 0;
        for (auto i = existingAds.begin() ; i != existingAds.end() ; i++)
            takenTime += (*i)->getAdInfo().getDuration();

        //if sum of existing ads' durations + this ad's duration exceeds maximum time limit
        if ( takenTime + ui.duration->text().toInt() > AdWidget::getMaxTotalAdsDuration() )
        {
            ui.errorLabel->setText("Przekroczono dostępny czas " +
                QString::number( AdWidget::getMaxTotalAdsDuration() - takenTime ) + "s" );
        }
        else
        {
            //check if ad's time boundaries won't intersect with other ads time boundaries
            for (auto i = existingAds.begin() ; i != existingAds.end() ; i++)
            {
                const Time& existStartHour = (*i)->getAdInfo().startHour;
                const Time& existEndHour = (*i)->getAdInfo().endHour;
                if ( ( startHour >= existStartHour && startHour <= existEndHour ) ||
                     ( endHour >= existStartHour && endHour <= existEndHour ) )
                {
                    ui.errorLabel->setText("O podanym czasie odtwarzana jest już inna reklama");
                    break;
                }
            }
        }
    }
}

void AdEditorDialog::updateAdCost()
{
    Time endTime = Time( ui.startHour->text() ) + Time( ui.duration->text().toInt() - 1 );
    double cost = AdWidget::calculateAdCost(ui.startHour->text(), endTime, weekDayNr);
    ui.cost->setText("Koszt " + QString::number(cost) + "zł");
}

void AdEditorDialog::initWeekDayName()
{
    QVector<QString> names{"Poniedziałek", "Wtorek", "Środa", "Czwartek", "Piątek", "Sobota", "Niedziela"};
    ui.weekName->setText(names[weekDayNr]);
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
        Request::RequestName reqName = ( !isNewAd && target != nullptr) ? Request::MODIFY_AD : Request::ADD_NEW_AD;
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

void AdEditorDialog::onRemoveAdResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć reklamy!");
        return;
    }

    emit removedAd(target);
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
