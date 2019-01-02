include(../defaults.pri)

TEMPLATE = app
CONFIG += c++14

INCLUDEPATH += $$PWD/../test/src
INCLUDEPATH += $$PWD/../src
INCLUDEPATH += $$PWD/../app

SOURCES += src/test_main.cpp \
    src/crtppacket_test.cpp \
    src/logic_test.cpp \
    src/math_function_test.cpp \
    src/toc_log_test.cpp \
    src/stl_utils_test.cpp \
    src/double_buffer_test.cpp \
    src/delay_test.cpp \
    src/pid_controller_test.cpp \
    src/camera_test.cpp \
     $$PWD/../app/imageprocess/coordinate_conversions.cpp

HEADERS  += \
    src/crtppacket_test.h \
    src/logic_test.h \
    src/math_function_test.h \
    src/toc_log_test.h \
    src/stl_utils_test.h \
    src/delay_test.h \
    src/pid_controller_test.h \
    src/camera_test.h \
     $$PWD/../app/imageprocess/camera.h


INCLUDEPATH += $${goolgetest_path}/include
LIBS += -L$${goolgetest_path} \
        -lgtest  \
        -lgtest_main


# Include code to test
LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie

# Include opencv
INCLUDEPATH += $${opencv_path}/include
opencv_lib_path = /usr/local/lib
LIBS += `pkg-config opencv --libs`


