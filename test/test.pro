include(../defaults.pri)
TEMPLATE = app
# Use C++ 14
CONFIG += c++14
#CONFIG += console
#CONFIG -= qt

INCLUDEPATH += test/src
INCLUDEPATH += src
INCLUDEPATH += $$PWD/../app

SOURCES += src/test_main.cpp \
#    src/crtppacket_test.cpp \
#    src/logic_test.cpp \
#    src/math_function_test.cpp \
#    src/toc_log_test.cpp \
#    src/stl_utils_test.cpp \
#    src/double_buffer_test.cpp \
#    src/delay_test.cpp \
#    src/pid_controller_test.cpp \
#    src/camera_test.cpp \
#     $$PWD/../app/imageprocess/coordinate_conversions.cpp

HEADERS  += \
#    src/crtppacket_test.h \
#    src/logic_test.h \
#    src/math_function_test.h \
#    src/toc_log_test.h \
#    src/stl_utils_test.h \
#    src/delay_test.h \
#    src/pid_controller_test.h \
#    src/camera_test.h
#     $$PWD/../app/imageprocess/camera.h

# Include Google Test
#INCLUDEPATH += $${goolgetest_path}/googletest-release-1.7.0/include \
#    $${goolgetest_path}/googletest-release-1.7.0 \
#    $${goolgetest_path}/gtestbuild-1.7.0
#SOURCES += $${goolgetest_path}/googletest-release-1.7.0/src/gtest-all.cc
#LIBS += -lgtest -L$${goolgetest_path}/gtestbuild-1.7.0

goolgetest_path = /usr/src/gtest
INCLUDEPATH += $${goolgetest_path}/include
#            += $${goolgetest_path} \
#SOURCES += $${goolgetest_path}/src/gtest-all.cc
#DEPENDPATH += $${goolgetest_path}/include \
LIBS += -L$${goolgetest_path} \
        -lgtest  \
        -lgtest_main


# Include code to test
LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie


INCLUDEPATH += $$PWD/../build/src/lib
DEPENDPATH += $$PWD/../build/src/lib
INCLUDEPATH += $$PWD/../build/app
DEPENDPATH += $$PWD/../build/app

# Include opencv 3.2.0
#INCLUDEPATH += $${opencv_path}/include
#opencv_lib_path = /usr/local/lib
#LIBS += `pkg-config opencv --libs`

#INCLUDEPATH += $${opencv_path}/include
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
