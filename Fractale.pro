#-------------------------------------------------
#
# Project created by QtCreator 2012-12-13T18:09:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fractale
TEMPLATE = app

SOURCES += main.cpp\
    fmainwindows.cpp \
    fractalewidget.cpp \
    cote.cpp \
    fractale.cpp

HEADERS  += fmainwindows.h \
    fractalewidget.h \
    cote.h \
    fractale.h

FORMS    += fmainwindows.ui

RESOURCES += ressource.qrc

linux-g++ {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path = $$PREFIX/bin

    desktop.path = $$PREFIX/share/applications/
    desktop.files += Fractale.desktop
    icon512.path = $$PREFIX/share/icons/hicolor/512x512/apps
    icon512.files += icon/Fractale.png
    appdata.path = $$PREFIX/share/metainfo
    appdata.files += Fractale.appdata.xml

    INSTALLS += appdata
    INSTALLS += icon512
    INSTALLS += desktop
    INSTALLS += target
}

