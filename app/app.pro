

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
    imageprocess/camera.cpp \
    gui/actual_values_model.cpp \
    gui/table_model_base.cpp \
    gui/parameter_model.cpp \
    gui/main_window.cpp \
    gui/tracking_color.cpp \
    gui/camera_view_painter.cpp \
    imageprocess/extract_color.cpp \
    qt_util.cpp \
    imageprocess/ball_kalman_filter.cpp \
    opencv_utils.cpp


HEADERS  += \
    qt_util.h \
    imageprocess/camera.h \
    opencv_utils.h \
    gui/actual_values_model.h \
    gui/table_model_base.h \
    gui/parameter_model.h \
    gui/tracking_color.h \
    gui/camera_view_painter.h \
    gui/main_window.h \
    imageprocess/extract_color.h \
    time_levels.h \
    imageprocess/ball_kalman_filter.h


FORMS    += \
    gui/main_window.ui

LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie

INCLUDEPATH += $$PWD/../build/src/lib
DEPENDPATH += $$PWD/../build/src/lib

# Include opencv 3.2.0
opencv_path =     "E:\Code\lib\opencv-3.2.0-build"
INCLUDEPATH += $${opencv_path}/install/include
LIBS += $${opencv_path}/bin/libopencv_calib3d320.dll
LIBS += $${opencv_path}/bin/libopencv_core320.dll
LIBS += $${opencv_path}/bin/libopencv_features2d320.dll
LIBS += $${opencv_path}/bin/libopencv_flann320.dll
LIBS += $${opencv_path}/bin/libopencv_highgui320.dll
LIBS += $${opencv_path}/bin/libopencv_imgcodecs320.dll
LIBS += $${opencv_path}/bin/libopencv_imgproc320.dll
LIBS += $${opencv_path}/bin/libopencv_ml320.dll
LIBS += $${opencv_path}/bin/libopencv_objdetect320.dll
LIBS += $${opencv_path}/bin/libopencv_photo320.dll
LIBS += $${opencv_path}/bin/libopencv_shape320.dll
LIBS += $${opencv_path}/bin/libopencv_stitching320.dll
LIBS += $${opencv_path}/bin/libopencv_superres320.dll
LIBS += $${opencv_path}/bin/libopencv_video320.dll
LIBS += $${opencv_path}/bin/libopencv_videoio320.dll
LIBS += $${opencv_path}/bin/libopencv_videostab320.dll
