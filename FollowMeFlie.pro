#-------------------------------------------------
#
# Project created by QtCreator 2017-04-03T21:23:14
#
#-------------------------------------------------

QT       += core gui
QT += multimedia
#QT += multimediawidgets # TODO Not sure if I need this one
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FollowMeFlie
TEMPLATE = app


# Use C++ 14
CONFIG += c++14
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=  src/gui/main.cpp \
    src/gui/mainwindow.cpp \
    src/communication/CCrazyflie.cpp \
    src/communication/CCrazyRadio.cpp \
    src/communication/CCRTPPacket.cpp \
    src/communication/CTOC.cpp

HEADERS  += src/gui/mainwindow.h \
    build/debug/moc_predefs.h \
    build/ui_mainwindow.h \
    src/communication/CCrazyflie.h \
    src/communication/CCrazyRadio.h \
    src/communication/CCRTPPacket.h \
    src/communication/CTOC.h \
    src/math/clock_gettime.h

FORMS    += src/gui/mainwindow.ui
DESTDIR = bin
INCLUDEPATH += src/gui
INCLUDEPATH += src/communication
INCLUDEPATH += src/math
INCLUDEPATH += src/control

INCLUDEPATH += $$PWD/../lib/libusb-1.0.21/include/libusb-1.0
INCLUDEPATH += $$PWD/../lib/libusb-1.0.21/include/$$PWD/
LIBS += -L$$PWD/"../lib/libusb-1.0.21/MinGW32/static" -llibusb-1.0

DEPENDPATH += $$PWD/../lib/libusb-1.0.21/MinGW32/static

