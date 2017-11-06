#-------------------------------------------------
#
# Project created by QtCreator 2016-12-19T18:09:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = network-disk
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    refreshthread.cpp \
    transportdialog.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    refreshthread.h \
    transportdialog.h

FORMS    += mainwindow.ui
QT +=network
