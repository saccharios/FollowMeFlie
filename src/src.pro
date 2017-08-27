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


SOURCES += communication/Crazyflie.cpp \
    communication/CrazyRadio.cpp \
    communication/CRTPPacket.cpp \
    communication/TOC.cpp \
    math/lowpass.cpp \
    math/pi_controller.cpp \
    control/commander.cpp \
    math/types.cpp


HEADERS  += communication/Crazyflie.h \
    communication/CrazyRadio.h \
    communication/CRTPPacket.h \
    communication/TOC.h \
    math/clock_gettime.h \
    math/stl_utils.h \
    math/logic.h \
    math/functions.h \
    math/constants.h \
    math/lowpass.h \
    math/pi_controller.h \
    control/commander.h \
    math/types.h


# Include library libusb-1.0.21
libubs_path = "E:\Code\lib\libusb-1.0.21"
INCLUDEPATH += $${libubs_path}/include/libusb-1.0
INCLUDEPATH += $${libubs_path}/include/$$PWD/
LIBS += -L$${libubs_path}/MinGW32/static -llibusb-1.0


DEPENDPATH += $${libubs_path}/MinGW32/static

