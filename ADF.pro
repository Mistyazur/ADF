#-------------------------------------------------
#
# Project created by QtCreator 2016-12-13T09:58:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ADF
TEMPLATE = app

include(Hotkey/Hotkey.pri)
include(DmPlugin/DmPlugin.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    grandiraider.cpp \
    df.cpp

HEADERS  += mainwindow.h \
    grandiraider.h \
    df.h

FORMS    += mainwindow.ui
