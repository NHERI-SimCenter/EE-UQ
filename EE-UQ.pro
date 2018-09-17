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

include(./EE-UQ.pri)
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../GroundMotionUtilities/UI/GroundMotionWidgets.pri)

SOURCES += main.cpp \
    InputWidgetEE_UQ.cpp \
    DakotaResults.cpp \
    DakotaResultsSampling.cpp \
    RunWidget.cpp

HEADERS  += \
    InputWidgetEE_UQ.h\
    DakotaResults.h \
    DakotaREsultsSampling.h \
    RunWidget.h 

RESOURCES += \
    images.qrc \

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



