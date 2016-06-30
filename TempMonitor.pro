#-------------------------------------------------
#
# Project created by QtCreator 2016-06-17T20:56:02
#
#-------------------------------------------------

QT       += core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TempMonitor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    temp.cpp

HEADERS  += mainwindow.h \
    temp.h

FORMS    += mainwindow.ui
