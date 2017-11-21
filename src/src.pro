include(../defaults.pri)
QT += core gui

TARGET = FollowMeFlie
TEMPLATE = lib

DESTDIR = lib

# Use C++ 14
CONFIG += c++14

DEFINES += FollowMeFlie_LIBRARY

INCLUDEPATH += communication \
    math \
    control


HEADERS  += \
    math/stl_utils.h \
    math/logic.h \
    math/functions.h \
    math/constants.h \
    math/lowpass.h \
    math/pi_controller.h \
    math/types.h \
    error_codes.h \
    communication/toc_log.h \
    communication/toc_parameter.h \
    communication/toc_shared.h \
    communication/protocol.h \
    control/crazy_flie_commander.h \
    communication/crtp_packet.h \
    communication/crazy_radio.h \
    communication/crazy_flie.h \
    communication/radio_dongle.h \
    control/double_buffer.h

SOURCES += \
    math/lowpass.cpp \
    math/pi_controller.cpp \
    math/types.cpp \
    communication/toc_log.cpp \
    communication/toc_parameter.cpp \
    communication/crazy_radio.cpp \
    communication/crazy_flie.cpp \
    communication/crtp_packet.cpp \
    control/crazy_flie_commander.cpp \
    communication/radio_dongle.cpp




# Include library libusb-1.0.21
libubs_path = "E:\Code\lib\libusb-1.0.21"
INCLUDEPATH += $${libubs_path}/include/libusb-1.0
INCLUDEPATH += $${libubs_path}/include/$$PWD/
LIBS += -L$${libubs_path}/MinGW32/static -llibusb-1.0


DEPENDPATH += $${libubs_path}/MinGW32/static

# Include Eigen library
INCLUDEPATH += "E:\Code\lib\eigen-3.3.4"

