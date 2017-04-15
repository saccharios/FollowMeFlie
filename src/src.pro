include(../defaults.pri)

CONFIG -= qt

TARGET = FollowMeFlie
TEMPLATE = lib

DESTDIR = lib

# Use C++ 14
CONFIG += c++14

DEFINES += FollowMeFlie_LIBRARY

INCLUDEPATH += communication \
    math \
    control

SOURCES += communication/CCrazyflie.cpp \
    communication/CCrazyRadio.cpp \
    communication/CCRTPPacket.cpp \
    communication/CTOC.cpp


HEADERS  += communication/CCrazyflie.h \
    communication/CCrazyRadio.h \
    communication/CCRTPPacket.h \
    communication/CTOC.h \
    math/clock_gettime.h


# Include library libusb-1.0.21
libubs_path = "E:\Code\lib\libusb-1.0.21"
INCLUDEPATH += $${libubs_path}/include/libusb-1.0
INCLUDEPATH += $${libubs_path}/include/$$PWD/
LIBS += -L$${libubs_path}/MinGW32/static -llibusb-1.0

DEPENDPATH += $${libubs_path}/MinGW32/static

