

QT += core gui
QT += multimedia
QT += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(../defaults.pri)
# Use C++ 14
CONFIG += c++14
TEMPLATE = app

CONFIG += console # Print console output in external console

SOURCES +=  main.cpp \
    mainwindow.cpp \
    crazyfliecaller.cpp \
    cameraviewpainter.cpp \
    imageprocessing.cpp \
    trackingcolor.cpp \
    camera.cpp


HEADERS  += mainwindow.h \
    crazyfliecaller.h \
    cameraviewpainter.h \
    qt_util.h \
    imageprocessing.h \
    trackingcolor.h \
    camera.h


FORMS    += mainwindow.ui

LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie

INCLUDEPATH += $$PWD/../build/src/lib
DEPENDPATH += $$PWD/../build/src/lib
