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
    imageprocess/camera.cpp


HEADERS  += communication/Crazyflie.h \
    communication/CrazyRadio.h \
    communication/CRTPPacket.h \
    communication/TOC.h \
    math/clock_gettime.h \
    math/stl_utils.h \
    math/logic.h \
    math/functions.h \
    math/constants.h \
    imageprocess/camera.h

# Include library libusb-1.0.21
libubs_path = "E:\Code\lib\libusb-1.0.21"
INCLUDEPATH += $${libubs_path}/include/libusb-1.0
INCLUDEPATH += $${libubs_path}/include/$$PWD/
LIBS += -L$${libubs_path}/MinGW32/static -llibusb-1.0

# Include opencv 3.2.0
#opencv_path =     "E:\Code\lib\opencv-3.2.0-build"
#INCLUDEPATH += $${opencv_path}/install/include
#LIBS += $${opencv_path}/bin/libopencv_calib3d320.dll
#LIBS += $${opencv_path}/bin/libopencv_core320.dll
#LIBS += $${opencv_path}/bin/libopencv_features2d320.dll
#LIBS += $${opencv_path}/bin/libopencv_flann320.dll
#LIBS += $${opencv_path}/bin/libopencv_highgui320.dll
#LIBS += $${opencv_path}/bin/libopencv_imgcodecs320.dll
#LIBS += $${opencv_path}/bin/libopencv_imgproc320.dll
#LIBS += $${opencv_path}/bin/libopencv_ml320.dll
#LIBS += $${opencv_path}/bin/libopencv_objdetect320.dll
#LIBS += $${opencv_path}/bin/libopencv_photo320.dll
#LIBS += $${opencv_path}/bin/libopencv_shape320.dll
#LIBS += $${opencv_path}/bin/libopencv_stitching320.dll
#LIBS += $${opencv_path}/bin/libopencv_superres320.dll
#LIBS += $${opencv_path}/bin/libopencv_video320.dll
#LIBS += $${opencv_path}/bin/libopencv_videoio320.dll
#LIBS += $${opencv_path}/bin/libopencv_videostab320.dll

DEPENDPATH += $${libubs_path}/MinGW32/static

