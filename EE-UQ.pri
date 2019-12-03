
#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD \
    $$PWD/StochasticMotionInput/include

INCLUDEPATH += "../EE-UQ"

SOURCES += \
    $$PWD/EDP_Selection.cpp \
    $$PWD/StandardEarthquakeEDP.cpp 

HEADERS  += \
    $$PWD/EDP_Selection.h \
    $$PWD/StandardEarthquakeEDP.h 

RESOURCES += \
    images.qrc \
    $$PWD/styles.qrc

#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc

