#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui charts concurrent network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE_UQ
TEMPLATE = app

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../Simcenter-EQSS/UI/GroundMotionWidgets.pri)

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
    MyTableWidget.cpp

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
    MyTableWidget.h


RESOURCES += \
    images.qrc \

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



