#-------------------------------------------------
#
# Project created by QtCreator 2016-12-13T09:58:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ADF
TEMPLATE = app
DESTDIR = ../ADF/Bin

include(JSettings/JSettings.pri)
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

RC_FILE = icon.rc

win32 {
CONFIG += embed_manifest_exe
QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
}
