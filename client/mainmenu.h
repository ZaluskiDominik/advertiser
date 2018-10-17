#ifndef MAINMENU_H
#define MAINMENU_H

#include <QPushButton>
#include <QHBoxLayout>

class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget* parent = nullptr);

private:
    class MenuOption;
    //main menu options
    QPushButton* openPriceListBtn;
    QPushButton* openAdsEditorBtn;
    QPushButton* openProfileBtn;

    QHBoxLayout* layout;

    //set's menu size and horizontal layout for children
    void initLayout();

    //adds all menu's options to main menu
    void initMenuOptions();

    //adds button which clicked opens a window
    void addMenuOption(QPushButton **menuOption, QString title);

signals:
    void priceListClicked();
    void adsEditorClicked();
    void profileClicked();

private slots:
    void onPriceListBtnClicked();
};

#endif // MAINMENU_H
