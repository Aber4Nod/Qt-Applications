#-------------------------------------------------
#
# Project created by QtCreator 2017-09-29T07:49:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets \
                                        printsupport

TARGET = AppryR2017
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
        interitem.cpp \
        qcustomplot.cpp \
    dialog.cpp

HEADERS  += mainwindow.h \
            interitem.h \
            qcustomplot.h \
    dialog.h

FORMS    += mainwindow.ui \
    dialog.ui
