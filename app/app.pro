

QT += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(../defaults.pri)
# Use C++ 14
CONFIG += c++14
TEMPLATE = app

CONFIG += console

SOURCES +=  main.cpp \
    mainwindow.cpp


HEADERS  += mainwindow.h


FORMS    += mainwindow.ui

LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie

INCLUDEPATH += $$PWD/../build/src/lib
DEPENDPATH += $$PWD/../build/src/lib
