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
    $$PWD/EVENTS/StochasticMotionInput/src/StochasticMotionInput.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/StochasticModelWidget.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/VlachosEtAlModel.cpp \
    $$PWD/EVENTS/StochasticMotionInput/src/DabaghiDerKiureghianPulse.cpp \
    $$PWD/EVENTS/peerNGA/ASCE710Target.cpp \
    $$PWD/EVENTS/peerNGA/NSHMPTarget.cpp \
    $$PWD/EVENTS/peerNGA/PeerLoginDialog.cpp \
    $$PWD/EVENTS/peerNGA/PEER_NGA_Records.cpp \
    $$PWD/EVENTS/peerNGA/RecordSelectionPlot.cpp \
    $$PWD/EVENTS/peerNGA/PeerNgaWest2Client.cpp \
    $$PWD/EVENTS/peerNGA/USGSTargetWidget.cpp \
    $$PWD/EVENTS/peerNGA/UserSpectrumModel.cpp \
    $$PWD/EVENTS/peerNGA/UserSpectrumWidget.cpp

HEADERS +=  \
    $$PWD/EVENTS/EarthquakeEventSelection.h \
    $$PWD/EVENTS/ExistingPEER_Records.h \
    $$PWD/EVENTS/StochasticMotionInput/include/StochasticMotionInput.h \
    $$PWD/EVENTS/StochasticMotionInput/include/StochasticModelWidget.h \
    $$PWD/EVENTS/StochasticMotionInput/include/VlachosEtAlModel.h \
    $$PWD/EVENTS/StochasticMotionInput/include/DabaghiDerKiureghianPulse.h \
    $$PWD/EVENTS/peerNGA/ASCE710Target.h \
    $$PWD/EVENTS/peerNGA/AbstractJsonSerializable.h \
    $$PWD/EVENTS/peerNGA/AbstractTargetWidget.h \
    $$PWD/EVENTS/peerNGA/NSHMPTarget.h \
    $$PWD/EVENTS/peerNGA/PeerLoginDialog.h \
    $$PWD/EVENTS/peerNGA/PEER_NGA_Records.h \
    $$PWD/EVENTS/peerNGA/RecordSelectionPlot.h \
    $$PWD/EVENTS/peerNGA/PeerNgaWest2Client.h \
    $$PWD/EVENTS/peerNGA/USGSTargetWidget.h \
    $$PWD/EVENTS/peerNGA/UserSpectrumModel.h \
    $$PWD/EVENTS/peerNGA/UserSpectrumWidget.h

