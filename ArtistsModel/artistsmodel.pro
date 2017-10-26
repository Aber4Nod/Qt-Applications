QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArtistsModel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    item.cpp \
    musicmodel.cpp \
    ratingdelegate.cpp \
    dialogalbum.cpp \
    dialogartist.cpp

HEADERS  += mainwindow.h \
    item.h \
    musicmodel.h \
    ratingdelegate.h \
    dialogalbum.h \
    dialogartist.h

FORMS    += mainwindow.ui \
    dialogalbum.ui \
    dialogartist.ui
