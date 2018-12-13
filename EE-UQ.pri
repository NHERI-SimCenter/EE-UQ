
#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD

INCLUDEPATH += "../EE-UQ"

SOURCES += \
    $$PWD/MainWindowWorkflowApp.cpp \
    $$PWD/InputWidgetSampling.cpp \
    $$PWD/UniformMotionInput.cpp \
    $$PWD/EarthquakeEventSelection.cpp \
    $$PWD/InputWidgetOpenSees.cpp \
    $$PWD/InputWidgetOpenSeesAnalysis.cpp \
    $$PWD/InputWidgetExistingEvent.cpp \
    $$PWD/SHAMotionWidget.cpp \
    $$PWD/OpenSeesParser.cpp \
    $$PWD/EarthquakeRecord.cpp \
    $$PWD/RunLocalWidget.cpp \
    $$PWD/MyTableWidget.cpp \
    $$PWD/AgaveCurl.cpp \
    $$PWD/Application.cpp \
    $$PWD/LocalApplication.cpp \
    $$PWD/RemoteApplication.cpp \
    $$PWD/RemoteService.cpp \
    $$PWD/RemoteJobManager.cpp \
    $$PWD/WorkflowAppWidget.cpp \
    $$PWD/ExistingSimCenterEvents.cpp \
    $$PWD/ExistingPEER_Events.cpp \
    $$PWD/CustomizedItemModel.cpp\
    $$PWD/DakotaResults.cpp\
    $$PWD/DakotaResultsSampling.cpp\
    $$PWD/UserDefinedApplication.cpp \
    $$PWD/SIM_Selection.cpp \
    $$PWD/InputWidgetBIM.cpp \
    $$PWD/InputWidgetUQ.cpp

HEADERS  += \
    $$PWD/MainWindowWorkflowApp.h\
    $$PWD/InputWidgetSampling.h \
    $$PWD/UniformMotionInput.h \
    $$PWD/EarthquakeEventSelection.h \
    $$PWD/InputWidgetOpenSees.h \
    $$PWD/InputWidgetOpenSeesAnalysis.h \
    $$PWD/InputWidgetExistingEvent.h \
    $$PWD/SHAMotionWidget.h \
    $$PWD/OpenSeesParser.h \
    $$PWD/EarthquakeRecord.h \
    $$PWD/RunLocalWidget.h \
    $$PWD/MyTableWidget.h \
    $$PWD/AgaveCurl.h \
    $$PWD/RemoteApplication.h \
    $$PWD/LocalApplication.h \
    $$PWD/Application.h \
    $$PWD/RemoteJobManager.h \
    $$PWD/RemoteService.h \
    $$PWD/WorkflowAppWidget.h \
    $$PWD/ExistingSimCenterEvents.h \
    $$PWD/CustomizedItemModel.h \
    $$PWD/DakotaResults.h\
    $$PWD/DakotaResultsSampling.h\
    $$PWD/ExistingPEER_Events.h \
    $$PWD/CustomizedItemModel.h \
    $$PWD/UserDefinedApplication.h \
    $$PWD/SIM_Selection.h \
    $$PWD/InputWidgetBIM.h \
    $$PWD/InputWidgetUQ.h

RESOURCES += \
    images.qrc \
    $$PWD/styles.qrc

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc

