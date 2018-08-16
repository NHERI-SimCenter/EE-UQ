#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE_UQ
TEMPLATE = app

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../Simcenter-EQSS/UI/GroundMotionWidgets.pri)

INCLUDEPATH+="../NHERI/SimCenterCommon/Common"
INCLUDEPATH+="../NHERI/SimCenterCommon/RandomVariables"
INCLUDEPATH+="../NHERI/SimCenterCommon/InputSheetBM"
INCLUDEPATH+=/Users/fmckenna/NHERI/SimCenterCommon/InputSheetBM
INCLUDEPATH+=/Users/fmckenna/NHERI/SimCenterCommon/RandomVariables

SOURCES += main.cpp\
        MainWindow.cpp\
        InputWidgetEE_UQ.cpp\
        InputWidgetSampling.cpp \
    UniformMotionInput.cpp \
    InputWidgetEarthquakeEvent.cpp \
    SHAMotionWidget.cpp

HEADERS  += MainWindow.h\
        InputWidgetEE_UQ.h\
        InputWidgetSampling.h \
    UniformMotionInput.h \
    InputWidgetEarthquakeEvent.h \
    SHAMotionWidget.h


#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



