#include "adsdialog.h"

AdsDialog::AdsDialog(QWidget *parent)
    :QDialog(parent)
{
    ui.setupUi(this);

    QObject::connect(ui.adsTable, SIGNAL(userAdsCostChanged(double)), this, SLOT(onUserAdsCostChanged(double)));
}

void AdsDialog::onUserAdsCostChanged(double newCost)
{
    ui.userAdsCost->setText("Koszt reklam: " + QString::number(newCost) + "zł");
}
