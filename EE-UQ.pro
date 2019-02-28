#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui charts concurrent network sql qml webenginewidgets uitools webengine webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE_UQ
TEMPLATE = app

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

include(./EE-UQ.pri)
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../GroundMotionUtilities/UI/GroundMotionWidgets.pri)
include(../SiteResponseTool/SiteResponseTool.pri)
include(./MiniZip/MiniZip.pri)

SOURCES += main.cpp \
    InputWidgetEE_UQ.cpp \
    RunWidget.cpp

HEADERS  += \
    InputWidgetEE_UQ.h\
    RunWidget.h 

RESOURCES += \
    images.qrc \

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



