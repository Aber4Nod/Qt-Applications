QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PuzzleGame
TEMPLATE = app


SOURCES += main.cpp\
    dialog.cpp \
    puzzlepiece.cpp \
    puzzlegame.cpp

HEADERS  += \
    dialog.h \
    puzzlepiece.h \
    puzzlegame.h

FORMS    += \
    dialog.ui
