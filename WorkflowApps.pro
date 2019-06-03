TEMPLATE = subdirs

SUBDIRS =   applications/createEDP/StandardEarthquakeEDP.pro \
            applications/createSAM/OpenSeesInput.pro \
            applications/createEVENT/MultipleSimCenterEvents.pro \
            applications/createEVENT/MultiplePEER_Events.pro \
            applications/performSIMULATION/OpenSeesPreprocessor.pro \
            applications/performSIMULATION/OpenSeesPostprocessor.pro \
            applications/performUQ/extractEDP.pro \
            applications/performUQ/postprocessDAKOTA.pro \
            applications/createEVENT/StochasticGroundMotion.pro \
            applications/createEVENT/SiteResponse.pro\
            applications/createSAM/MDOF_BuildingModel.pro

