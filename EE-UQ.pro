#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT += core gui charts concurrent network sql qml webenginewidgets uitools webengine webchannel 3dcore 3drender 3dextras charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

MOC_DIR = $$OUT_PWD/.moc
UI_DIR = $$OUT_PWD/.ui
OBJECTS_DIR = $$OUT_PWD/.obj
RCC_DIR = $$OUT_PWD/.rcc

TARGET = EE_UQ
TEMPLATE = app

VERSION=2.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include($$PWD/ConanHelper.pri)

win32{
    LIBS = $$replace(LIBS, .dll.lib, .dll)
    LIBS += -lAdvapi32
}


linux{
    CONFIG += no_lflags_merge
    LIBS += -llapack -lgfortran 
}

win32 {
    RC_ICONS = icons/NHERI-EEUQ-Icon.ico
} else {
    mac {
    ICON = icons/NHERI-EEUQ-Icon.icns
    }
}


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


OTHER_FILES += conanfile.py

