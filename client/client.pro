#-------------------------------------------------
#
# Project created by QtCreator 2012-04-09T21:55:38
#
#-------------------------------------------------

QT       += core gui network xml

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectdialog.cpp \
    pngui.cpp \
    aboutdialog.cpp \
    editdialog.cpp \
    ../server/simstate.cpp \
    ../server/pnplace.cpp \
    ../server/pntrans.cpp \
    ../server/constraint.cpp \
    communicator.cpp \
    propertiesdialog.cpp \
    loadsimdialog.cpp

HEADERS  += mainwindow.h \
    connectdialog.h \
    pngui.h \
    aboutdialog.h \
    editdialog.h \
    ../server/simstate.h \
    ../server/pnplace.h \
    ../server/pntrans.h \
    ../server/constraint.h \
    communicator.h \
    propertiesdialog.h \
    loadsimdialog.h

FORMS    += mainwindow.ui \
    connectdialog.ui \
    aboutdialog.ui \
    editdialog.ui \
    propertiesdialog.ui \
    loadsimdialog.ui

OTHER_FILES += \
    doc/protokol \
    typologo.png






