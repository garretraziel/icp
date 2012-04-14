#-------------------------------------------------
#
# Project created by QtCreator 2012-04-09T22:04:14
#
#-------------------------------------------------

QT       += core network xml

QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    petrinetserver.cpp \
    petrisim.cpp \
    pnplace.cpp \
    pntrans.cpp \
    simstate.cpp \
    constraint.cpp

HEADERS += \
    petrinetserver.h \
    petrisim.h \
    pnplace.h \
    pntrans.h \
    simstate.h \
    constraint.h

OTHER_FILES += \
    doc/protokol \
    doc/xml


