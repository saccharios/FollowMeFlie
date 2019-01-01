

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
    opencv_utils.cpp \
    imageprocess/ball_kalman_filter_2d.cpp \
    imageprocess/ball_kalman_filter_3d.cpp \
    imageprocess/coordinate_conversions.cpp \
    gui/set_point_dialog.cpp

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
    imageprocess/ball_kalman_filter_2d.h \
    imageprocess/ball_kalman_filter_3d.h \
    gui/set_point_dialog.h

FORMS    += \
    gui/main_window.ui \
    gui/set_point_dialog.ui

LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie

INCLUDEPATH += $$PWD/../build/src/lib
DEPENDPATH += $$PWD/../build/src/lib

# Include opencv 3.2.0

INCLUDEPATH += $${opencv_path}/include

opencv_lib_path = /usr/local/lib
#LIBS += `pkg-config opencv --libs`

#LIBS += -L$${opencv_lib_path} \
#        -lopencv_calib3d \
#        -lopencv_core \
#        -lopencv_dnn \
#        -lopencv_features2d \
#        -lopencv_flann \
#        -lopencv_gapi \
#        -lopencv_highgui \
#        -lopencv_imgcodecs \
#        -lopencv_imgproc \
#        -lopencv_ml \
#        -lopencv_objdetect \
#        -lopencv_photo \
#        -lopencv_stitching \
#        -lopencv_video \
#        -lopencv_videoio
