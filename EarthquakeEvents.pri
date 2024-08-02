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
    $$PWD/EVENTS/peerNGA/NoSpectrumUniform.cpp \
    $$PWD/EVENTS/peerNGA/NSHMPDeagg.cpp \
    $$PWD/EVENTS/peerNGA/NSHMPTarget.cpp \
    $$PWD/EVENTS/peerNGA/PeerLoginDialog.cpp \
    $$PWD/EVENTS/peerNGA/PEER_NGA_Records.cpp \
    $$PWD/EVENTS/peerNGA/RecordSelectionPlot.cpp \
    $$PWD/EVENTS/peerNGA/PeerNgaWest2Client.cpp \
    $$PWD/EVENTS/peerNGA/SpectrumFromRegionalSurrogate.cpp \
    $$PWD/EVENTS/peerNGA/USGSTargetWidget.cpp \
    $$PWD/EVENTS/peerNGA/UserSpectrumModel.cpp \
    $$PWD/EVENTS/peerNGA/UserSpectrumWidget.cpp \
    $$PWD/EVENTS/peerNGA/surrogateGpParser.cpp \
    $$PWD/EVENTS/physicsBasedSimulation/PhysicsBasedMotionSelection.cpp \
    $$PWD/EVENTS/physicsBasedSimulation/M9SingleSite.cpp \
    $$PWD/EVENTS/physicsBasedSimulation/IstanbulSingleSite.cpp \
    $$PWD/EVENTS/userDefinedDatabase/User_Defined_Database.cpp \
    $$PWD/EVENTS/groundMotionModel/BakerJayaram2008.cpp

HEADERS +=  \
    $$PWD/EVENTS/EarthquakeEventSelection.h \
    $$PWD/EVENTS/ExistingPEER_Records.h \
    $$PWD/EVENTS/StochasticMotionInput/include/StochasticMotionInput.h \
    $$PWD/EVENTS/StochasticMotionInput/include/StochasticModelWidget.h \
    $$PWD/EVENTS/StochasticMotionInput/include/VlachosEtAlModel.h \
    $$PWD/EVENTS/StochasticMotionInput/include/DabaghiDerKiureghianPulse.h \
    $$PWD/EVENTS/peerNGA/ASCE710Target.h \
    $$PWD/EVENTS/peerNGA/NoSpectrumUniform.h \
    $$PWD/EVENTS/peerNGA/AbstractJsonSerializable.h \
    $$PWD/EVENTS/peerNGA/AbstractTargetWidget.h \
    $$PWD/EVENTS/peerNGA/NSHMPDeagg.h \
    $$PWD/EVENTS/peerNGA/NSHMPTarget.h \
    $$PWD/EVENTS/peerNGA/PeerLoginDialog.h \
    $$PWD/EVENTS/peerNGA/PEER_NGA_Records.h \
    $$PWD/EVENTS/peerNGA/RecordSelectionPlot.h \
    $$PWD/EVENTS/peerNGA/PeerNgaWest2Client.h \
    $$PWD/EVENTS/peerNGA/SpectrumFromRegionalSurrogate.h \
    $$PWD/EVENTS/peerNGA/USGSTargetWidget.h \
    $$PWD/EVENTS/peerNGA/UserSpectrumModel.h \
    $$PWD/EVENTS/peerNGA/UserSpectrumWidget.h \
    $$PWD/EVENTS/peerNGA/surrogateGpParser.h \
    $$PWD/EVENTS/physicsBasedSimulation/PhysicsBasedMotionSelection.h \
    $$PWD/EVENTS/physicsBasedSimulation/M9SingleSite.h \  
    $$PWD/EVENTS/physicsBasedSimulation/IstanbulSingleSite.h \  
    $$PWD/EVENTS/userDefinedDatabase/User_Defined_Database.h \
    $$PWD/EVENTS/groundMotionModel/BakerJayaram2008.h

