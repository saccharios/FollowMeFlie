include(../defaults.pri)
QT += core gui

TARGET = FollowMeFlie
TEMPLATE = lib

DESTDIR = lib

# Use C++ 14
CONFIG += c++14

DEFINES += FollowMeFlie_LIBRARY

INCLUDEPATH += crazyflie \
    math


HEADERS  += \
    math/types.h \
    math/stl_utils.h \
    math/logic.h \
    math/functions.h \
    math/constants.h \
    math/lowpass.h \
    math/pi_controller.h \
    error_codes.h \
    crazyflie/toc_log.h \
    crazyflie/toc_parameter.h \
    crazyflie/toc_shared.h \
    crazyflie/protocol.h \
    crazyflie/crazy_flie_commander.h \
    crazyflie/crtp_packet.h \
    crazyflie/crazy_flie.h \
    crazyflie/radio_dongle.h \
    math/double_buffer.h \
    crazyflie/error_codes.h \
    math/kalman_filter.h \
    math/delay.h

SOURCES += \
    math/lowpass.cpp \
    math/pi_controller.cpp \
    math/types.cpp \
    crazyflie/toc_log.cpp \
    crazyflie/toc_parameter.cpp \
    crazyflie/crazy_flie.cpp \
    crazyflie/crtp_packet.cpp \
    crazyflie/crazy_flie_commander.cpp \
    crazyflie/radio_dongle.cpp \
    math/kalman_filter.cpp




# Include library libusb-1.0.21
libubs_path = "E:\Code\lib\libusb-1.0.21"
INCLUDEPATH += $${libubs_path}/include/libusb-1.0
INCLUDEPATH += $${libubs_path}/include/$$PWD/
LIBS += -L$${libubs_path}/MinGW32/static -llibusb-1.0


DEPENDPATH += $${libubs_path}/MinGW32/static

# Include Eigen library
INCLUDEPATH += "E:\Code\lib\eigen-3.3.4"

