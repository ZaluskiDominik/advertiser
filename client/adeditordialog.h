#ifndef ADEDITORDIALOG_H
#define ADEDITORDIALOG_H

#include <QDialog>
#include "ui_adeditordialog.h"
#include "adwidget.h"
#include "../shared/requestreceiver.h"
#include "../shared/pricelist.h"

class AdEditorDialog : public QDialog, RequestReceiver
{
    Q_OBJECT

public:
    //constructor for a new ad
    explicit AdEditorDialog(Time _minTime, Time _maxTime, const QVector<AdWidget*>& _existingAds, int _weekDayNr, QWidget* parent = nullptr);
    //constuctor for an existing ad
    explicit AdEditorDialog(Time _minTime, Time _maxTime, const QVector<AdWidget*>& _existingAds, AdWidget* _targetAd, QWidget* parent = nullptr);

    //sets AdWidget that will be affected by changes entered by user
    void setTargetAd(AdWidget* targetAd);

protected:
    void onDataReceived(Request req, SocketHandler*) final;
    void registerRequestsReceiver(SocketHandler* socketHandler) final;

    bool eventFilter(QObject *target, QEvent *event);

private:
    Ui::AdEditorDialog ui;

    //whether it's a new ad to add or it't only modification to existing one
    bool isNewAd;

    int weekDayNr;

    //if ad is edited, it's a pointer holding target ad widget of this changes
    AdWidget* target;

    //vector of created ads
    const QVector<AdWidget*>& existingAds;

    //time boundaries for ad
    Time minTime, maxTime;

    //returns AdInfo object created from line edit's data
    AdInfo convertToAdInfo();

    //verify user's input
    void validateAd();

    //recalculates ad's cost
    void updateAdCost();

    void initWeekDayName();

private slots:    
    //save button was clicked, save changes made to ad
    void on_saveBtn_clicked();

    //remove button was clicked, remove ad
    void on_removeBtn_clicked();

    //response from server to requesting adding new ad
    void onAddNewAdResponse(Request& req);

    //response from server to requesting removal of ad
    void onRemoveAdResponse(Request& req);

    //start hour of an ad changed
    void on_startHour_editingFinished();

    void on_startHour_cursorPositionChanged(int oldPos, int newPos);

    void on_duration_editingFinished();

    void on_duration_valueChanged(const QString &);

    void on_startHour_textEdited(const QString &);

signals:
    void addedNewAd(AdInfo, AdEditorDialog*);
    void savedChanges(AdWidget*);
    void removedAd(AdWidget*);
};

#endif // ADEDITORDIALOG_H
