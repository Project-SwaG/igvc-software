
QT       += testlib xml core gui declarative

TARGET = IGVC_Test
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

INCLUDEPATH += ../src/ \
    ../src/gui/

TEMPLATE = app

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += ../src/tests/testmain.cpp \
    ../src/common/utils/StringUtils.cpp \
    ../src/intelligence/posetracking/positiontracker.cpp \
    ../src/common/config/configmanager.cpp \
    ../src/common/logger/logger.cpp \
    ../src/common/utils/gpsfilereader.cpp \
    ../src/hardware/sensors/gps/nmeacompatiblegps.cpp \
    ../src/hardware/serial/ASIOSerialPort.cpp \
    ../src/hardware/sensors/gps/nmea.cpp \
    ../src/intelligence/posetracking/basicpositiontracker.cpp

HEADERS += ../src/tests/teststringutils.hpp \
    ../src/common/utils/StringUtils.hpp \
    ../src/tests/testpositiontracker.hpp \
    ../src/intelligence/posetracking/positiontracker.h \
    ../src/common/config/configmanager.h \
    ../src/common/logger/logger.h \
    ../src/tests/testgpsutils.h \
    ../src/common/utils/GPSUtils.h \
    ../src/common/utils/AngleUtils.h \
    ../src/tests/testangleutils.h \
    ../src/tests/testgpsreader.hpp \
    ../src/common/logger/logger.h \
    ../src/common/utils/gpsfilereader.h \
    ../src/tests/testangleutils.h \
    ../src/common/utils/gpsfilereader.h \
    ../src/tests/capturegpsandpostracker.hpp \
    ../src/hardware/sensors/gps/nmeacompatiblegps.h \
    ../src/hardware/serial/ASIOSerialPort.h \
    ../src/hardware/sensors/gps/nmea.hpp \
    ../src/intelligence/posetracking/basicpositiontracker.h

# libUSB (for LIDAR)

LIBS += -L/usr/lib/x86_64-linux-gnu/ -lusb-1.0

INCLUDEPATH += /usr/lib/x86_64-linux-gnu
DEPENDPATH += /usr/lib/x86_64-linux-gnu

PRE_TARGETDEPS += /usr/lib/x86_64-linux-gnu/libusb-1.0.a

# BOOST

LIBS += -L/usr/lib/ -lboost_thread -lboost_system

PRE_TARGETDEPS += /usr/lib/libboost_thread.a
PRE_TARGETDEPS += /usr/lib/libboost_system.a

# PCL
INCLUDEPATH += /usr/include/pcl-1.7
DEPENDPATH += /usr/include/pcl-1.7

LIBS += -L/usr/lib -lpcl_common -lpcl_visualization -lpcl_kdtree

# VTK (PCL Dependency)
INCLUDEPATH += /usr/include/vtk-5.8
DEPENDPATH += /usr/include/vtk-5.8

LIBS += -L/usr/lib/ -lvtkCommon

# Eigen (header-only library)
INCLUDEPATH += /usr/include/eigen3
DEPENDPATH += /usr/include/eigen3

# OpenCV
INCLUDEPATH += /usr/include/
DEPENDPATH += /usr/include/

LIBS += -L/usr/lib/ -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_highgui

# FlyCapture2
INCLUDEPATH += /usr/include/
DEPENDPATH += /usr/include/

LIBS += -L/usr/lib/ -lflycapture

#SICK Toolbox
#NOTE : Toolbox depends on pthread library
INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

LIBS += -L/usr/local/lib -lsicklms-1.0 -pthread
