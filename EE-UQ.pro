#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui charts concurrent network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE_UQ
TEMPLATE = app

macos:LIBS += /usr/lib/libcurl.dylib
win32:INCLUDEPATH += "c:\Users\SimCenter\libCurl-7.59.0\include"
win32:LIBS += C:\Users\SimCenter\libCurl-7.59.0/lib/libcurl.lib

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../GroundMotionUtilities/UI/GroundMotionWidgets.pri)

SOURCES += main.cpp\
    MainWindow.cpp\
    InputWidgetEE_UQ.cpp\
    InputWidgetSampling.cpp \
    UniformMotionInput.cpp \
    InputWidgetEarthquakeEvent.cpp \
    InputWidgetBIM_Selection.cpp \
    InputWidgetOpenSees.cpp \
    InputWidgetOpenSeesAnalysis.cpp \
    InputWidgetExistingEvent.cpp \
    SHAMotionWidget.cpp \
    OpenSeesParser.cpp \
    EarthquakeRecord.cpp \
    RunLocalWidget.cpp \
    DakotaResults.cpp \
    DakotaResultsSampling.cpp \
    MyTableWidget.cpp \
    AgaveCurl.cpp \
    RemoteApplication.cpp

HEADERS  += MainWindow.h\
    InputWidgetEE_UQ.h\
    InputWidgetSampling.h \
    UniformMotionInput.h \
    InputWidgetEarthquakeEvent.h \
    InputWidgetBIM_Selection.h \
    InputWidgetOpenSees.h \
    InputWidgetOpenSeesAnalysis.h \
    InputWidgetExistingEvent.h \
    SHAMotionWidget.h \
    OpenSeesParser.h \
    EarthquakeRecord.h \
    RunLocalWidget.h \
    DakotaResults.h \
    DakotaResultsSampling.h \
    MyTableWidget.h \
    AgaveCurl.h \
    RemoteApplication.h


RESOURCES += \
    images.qrc \

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



