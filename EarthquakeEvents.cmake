# EarthquakeEvents.cmake
# Usage:
#   include(path/to/EarthquakeEvents.cmake)
#   simcenter_add_earthquake_events(<target>)

set(EVENTS_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(simcenter_add_earthquake_events target)
  set(dir "${EVENTS_MODULE_DIR}/EVENTS")
  
  target_include_directories(${target} PRIVATE
    "${dir}"
    "${dir}/StochasticMotionInput/include"
  )  

  set(SOURCES
    "${dir}/EarthquakeEventSelection.cpp"
    "${dir}/ExistingPEER_Records.cpp"
    "${dir}/StochasticMotionInput/src/StochasticMotionInput.cpp"
    "${dir}/StochasticMotionInput/src/StochasticModelWidget.cpp"
    "${dir}/StochasticMotionInput/src/VlachosEtAlModel.cpp"
    "${dir}/StochasticMotionInput/src/DabaghiDerKiureghianPulse.cpp"
    "${dir}/peerNGA/ASCE710Target.cpp"
    "${dir}/peerNGA/NoSpectrumUniform.cpp"
    "${dir}/peerNGA/NSHMPDeagg.cpp"
    "${dir}/peerNGA/NSHMPTarget.cpp"
    "${dir}/peerNGA/PeerLoginDialog.cpp"
    "${dir}/peerNGA/PEER_NGA_Records.cpp"
    "${dir}/peerNGA/RecordSelectionPlot.cpp"
    "${dir}/peerNGA/PeerNgaWest2Client.cpp"
    "${dir}/peerNGA/SpectrumFromRegionalSurrogate.cpp"
    "${dir}/peerNGA/USGSTargetWidget.cpp"
    "${dir}/peerNGA/UserSpectrumModel.cpp"
    "${dir}/peerNGA/UserSpectrumWidget.cpp"
    "${dir}/peerNGA/surrogateGpParser.cpp"
    "${dir}/physicsBasedSimulation/PhysicsBasedMotionSelection.cpp"
    "${dir}/physicsBasedSimulation/M9SingleSite.cpp"
    "${dir}/physicsBasedSimulation/IstanbulSingleSite.cpp"
    "${dir}/userDefinedDatabase/User_Defined_Database.cpp"
    "${dir}/groundMotionModel/BakerJayaram2008.cpp"
    "${dir}/drmEvent/drmEvent.cpp"
    "${dir}/drmEvent/drmLocalEventWidget.cpp"
    "${dir}/drmEvent/drmPredefinedEventWidget.cpp"
  )

  set(HEADERS
    "${dir}/EarthquakeEventSelection.h"
    "${dir}/ExistingPEER_Records.h"
    "${dir}/StochasticMotionInput/include/StochasticMotionInput.h"
    "${dir}/StochasticMotionInput/include/StochasticModelWidget.h"
    "${dir}/StochasticMotionInput/include/VlachosEtAlModel.h"
    "${dir}/StochasticMotionInput/include/DabaghiDerKiureghianPulse.h"
    "${dir}/peerNGA/ASCE710Target.h"
    "${dir}/peerNGA/NoSpectrumUniform.h"
    "${dir}/peerNGA/AbstractJsonSerializable.h"
    "${dir}/peerNGA/AbstractTargetWidget.h"
    "${dir}/peerNGA/NSHMPDeagg.h"
    "${dir}/peerNGA/NSHMPTarget.h"
    "${dir}/peerNGA/PeerLoginDialog.h"
    "${dir}/peerNGA/PEER_NGA_Records.h"
    "${dir}/peerNGA/RecordSelectionPlot.h"
    "${dir}/peerNGA/PeerNgaWest2Client.h"
    "${dir}/peerNGA/SpectrumFromRegionalSurrogate.h"
    "${dir}/peerNGA/USGSTargetWidget.h"
    "${dir}/peerNGA/UserSpectrumModel.h"
    "${dir}/peerNGA/UserSpectrumWidget.h"
    "${dir}/peerNGA/surrogateGpParser.h"
    "${dir}/physicsBasedSimulation/PhysicsBasedMotionSelection.h"
    "${dir}/physicsBasedSimulation/M9SingleSite.h"  
    "${dir}/physicsBasedSimulation/IstanbulSingleSite.h"  
    "${dir}/userDefinedDatabase/User_Defined_Database.h"
    "${dir}/groundMotionModel/BakerJayaram2008.h"
    "${dir}/drmEvent/drmEvent.h"
    "${dir}/drmEvent/drmLocalEventWidget.h"
    "${dir}/drmEvent/drmPredefinedEventWidget.h"
  )

  target_sources(${target} PRIVATE ${SOURCES} ${HEADERS})
  

endfunction()

