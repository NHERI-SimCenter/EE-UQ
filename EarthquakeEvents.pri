#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD/EVENTS
INCLUDEPATH += $$PWD/EVENTS/StochasticMotionInput/include

SOURCES +=  \
    $$PWD/EVENTS/EarthquakeEventSelection.cpp \
    $$PWD/EVENTS/ExistingPEER_Records.cpp \
    $$PWD/EVENTS/SHAMotionWidget.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/StochasticMotionInput.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/StochasticModelWidget.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/VlachosEtAlModel.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/DabaghiDerKiureghianPulse.cpp \
    $$PWD/EVENTS/peerNGA/PeerLoginDialog.cpp \
    $$PWD/EVENTS/peerNGA/PEER_NGA_Records.cpp \
    $$PWD/EVENTS/peerNGA/RecordSelectionPlot.cpp \
    $$PWD/EVENTS/peerNGA/PeerNgaWest2Client.cpp

HEADERS +=  \
    $$PWD/EVENTS/EarthquakeEventSelection.h \
    $$PWD/EVENTS/ExistingPEER_Records.h \
    $$PWD/EVENTS/SHAMotionWidget.h \
    $$PWD/EVENTS/StochasticMotionInput/include/StochasticMotionInput.h \
    $$PWD/EVENTS/StochasticMotionInput/include/StochasticModelWidget.h \
    $$PWD/EVENTS/StochasticMotionInput/include/VlachosEtAlModel.h \
    $$PWD/EVENTS/StochasticMotionInput/include/DabaghiDerKiureghianPulse.h \
    $$PWD/EVENTS/peerNGA/PeerLoginDialog.h \
    $$PWD/EVENTS/peerNGA/PEER_NGA_Records.h \
    $$PWD/EVENTS/peerNGA/RecordSelectionPlot.h \
    $$PWD/EVENTS/peerNGA/PeerNgaWest2Client.h

