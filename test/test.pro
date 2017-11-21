include(../defaults.pri)
TEMPLATE = app
# Use C++ 14
CONFIG += c++14
#CONFIG += console
#CONFIG -= qt

INCLUDEPATH += test/src
INCLUDEPATH += src

SOURCES += src/test_main.cpp \
    src/crtppacket_test.cpp \
    src/logic_test.cpp \
    src/math_function_test.cpp \
    src/toc_log_test.cpp \
    src/stl_utils_test.cpp \
    src/double_buffer_test.cpp

HEADERS  += \
    src/crtppacket_test.h \
    src/logic_test.h \
    src/math_function_test.h \
    src/toc_log_test.h \
    src/stl_utils_test.h

# Include Google Test
goolgetest_path = E:/Code/GTest
INCLUDEPATH += $${goolgetest_path}/googletest-release-1.7.0/include \
    $${goolgetest_path}/googletest-release-1.7.0 \
    $${goolgetest_path}/gtestbuild-1.7.0
SOURCES += $${goolgetest_path}/googletest-release-1.7.0/src/gtest-all.cc
LIBS += -lgtest -L$${goolgetest_path}/gtestbuild-1.7.0


# Include code to test
LIBS += -L$$OUT_PWD/../src/lib/ -lFollowMeFlie
INCLUDEPATH += $$PWD/../build/src/lib
DEPENDPATH += $$PWD/../build/src/lib


