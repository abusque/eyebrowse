#-------------------------------------------------
#
# Project created by QtCreator 2013-10-06T02:26:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EyeBrowse-Config
TEMPLATE = app


SOURCES += main.cpp\
        configurator.cpp

LIBS     += -l config++

HEADERS  += configurator.h

FORMS    += configurator.ui
