#-------------------------------------------------
#
# Project created by QtCreator 2012-04-09T21:55:38
#
#-------------------------------------------------

QT       += core gui network

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectdialog.cpp \
    pngui.cpp \
    aboutdialog.cpp \
    editdialog.cpp

HEADERS  += mainwindow.h \
    connectdialog.h \
    pngui.h \
    aboutdialog.h \
    editdialog.h

FORMS    += mainwindow.ui \
    connectdialog.ui \
    aboutdialog.ui \
    editdialog.ui

OTHER_FILES += \
    doc/protokol \
    typologo.png
