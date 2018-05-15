#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE_UQ
TEMPLATE = app

include(../widgets/Common/Common.pri)
include(../widgets/RandomVariables/RandomVariables.pri)
include(../widgets/InputSheetBM/InputSheetBM.pri)
include(../Simcenter-EQSS/UI/GroundMotionWidgets.pri)

INCLUDEPATH+="../NHERI/widgets/Common"
INCLUDEPATH+="../NHERI/widgets/RandomVariables"
INCLUDEPATH+="../NHERI/widgets/InputSheetBM"
INCLUDEPATH+=/Users/fmckenna/NHERI/widgets/InputSheetBM
INCLUDEPATH+=/Users/fmckenna/NHERI/widgets/RandomVariables

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



