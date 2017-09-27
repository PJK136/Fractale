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

RESOURCES += \
    ressource.qrc
