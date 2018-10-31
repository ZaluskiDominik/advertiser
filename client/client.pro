#-------------------------------------------------
#
# Project created by QtCreator 2018-10-10T14:46:45
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    loginwidget.cpp \
    ../shared/requests.cpp \
    ../shared/sockethandler.cpp \
    mainwidget.cpp \
    ../shared/requestreceiver.cpp \
    userdata.cpp \
    profiledialog.cpp \
    mainmenu.cpp \
    admindialog.cpp \
    userswidget.cpp \
    columnlistwidgetfield.cpp \
    columnlistwidgetcolumn.cpp \
    columnlistwidget.cpp \
    ../shared/pricelist.cpp \
    pricelistwidget.cpp \
    allpricelistswidget.cpp \
    pricelisttab.cpp

HEADERS += \
        mainwindow.h \
    loginwidget.h \
    ../shared/requestreceiver.h \
    ../shared/requests.h \
    ../shared/sockethandler.h \
    mainwidget.h \
    userdata.h \
    profiledialog.h \
    mainmenu.h \
    admindialog.h \
    userswidget.h \
    columnlistwidgetfield.h \
    columnlistwidgetcolumn.h \
    columnlistwidget.h \
    ../shared/pricelist.h \
    pricelistwidget.h \
    allpricelistswidget.h \
    pricelisttab.h

FORMS += \
        mainwindow.ui \
    loginwidget.ui \
    mainwidget.ui \
    profiledialog.ui \
    admindialog.ui \
    userswidget.ui \
    pricelistwidget.ui \
    allpricelistswidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
