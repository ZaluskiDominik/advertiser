#include "mainmenu.h"
#include <QLabel>

MainMenu::MainMenu(QWidget* parent)
    :QWidget(parent)
{
    initLayout();
    initMenuOptions();
}

void MainMenu::showAdminPanelMenu(bool show)
{
    openAdminPanelBtn->setVisible(show);
}

void MainMenu::initLayout()
{
    layout = new QHBoxLayout;
    layout->setContentsMargins(0, 3, 0, 3);
    layout->setSpacing(10);
    setLayout(layout);
    setGeometry(0, 0, parentWidget()->width(), 20);
}

void MainMenu::initMenuOptions()
{
    addMenuOption(&openPriceListBtn, "Cennik");
    QObject::connect(openPriceListBtn, SIGNAL(clicked()), this, SIGNAL(priceListClicked()));

    addMenuOption(&openAdsEditorBtn, "Reklamy");
    QObject::connect(openAdsEditorBtn, SIGNAL(clicked()), this, SIGNAL(adsEditorClicked()));

    addMenuOption(&openProfileBtn, "Profil");
    QObject::connect(openProfileBtn, SIGNAL(clicked()), this, SIGNAL(profileClicked()));

    addMenuOption(&openAdminPanelBtn, "Panel admina");
    QObject::connect(openAdminPanelBtn, SIGNAL(clicked()), this, SIGNAL(adminPanelClicked()));

    QWidget* spaceFiller = new QWidget;
    spaceFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spaceFiller);

    //menu options stylesheet
    setStyleSheet("QPushButton {border: 1px solid transparent; border-radius: 4px; padding: 5px;}\
         QPushButton:hover {background-color: #ddd; border: 1px solid white;}\
         QPushButton:pressed {font-weight:bold}");
}

void MainMenu::addMenuOption(QPushButton** menuOption, QString title)
{
    *menuOption = new QPushButton(title);
    (*menuOption)->setFocusPolicy(Qt::NoFocus);
    (*menuOption)->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    (*menuOption)->show();
    layout->addWidget(*menuOption);
    layout->setAlignment(*menuOption, Qt::AlignLeft);
}
