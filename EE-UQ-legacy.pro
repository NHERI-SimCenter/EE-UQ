#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT += core gui charts concurrent network sql qml webenginewidgets uitools webengine webchannel 3dcore 3drender 3dextras charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE_UQ
TEMPLATE = app

VERSION=2.2.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

win32 {
    RC_ICONS = icons/NHERI-EEUQ-Icon.ico
} else {
    mac {
    ICON = icons/NHERI-EEUQ-Icon.icns
    }
}

macos:LIBS += /usr/lib/libcurl.dylib -llapack -lblas
win32:INCLUDEPATH += "c:\Users\SimCenter\libCurl-7.59.0\include"
win32:LIBS += C:\Users\SimCenter\libCurl-7.59.0/lib/libcurl.lib
linux:LIBS += /usr/lib/x86_64-linux-gnu/libcurl.so

win32:INCLUDEPATH += "..\jansson\build\include"
win32:LIBS += "..\jansson\build\lib\release\jansson.lib"

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/Workflow.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../GroundMotionUtilities/UI/GroundMotionWidgets.pri)
include(../Qs3hark/Qs3hark.pri)
include(./EarthquakeEvents.pri)

SOURCES += main.cpp \
    WorkflowAppEE_UQ.cpp \
    RunWidget.cpp

HEADERS  += \
    WorkflowAppEE_UQ.h\
    RunWidget.h 

RESOURCES += \
    images.qrc \
    $$PWD/styles.qrc

DISTFILES += \
    resources/docs/textAboutEEUQ.html

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



