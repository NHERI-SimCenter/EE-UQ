# EE-UQ

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3475642.svg)](https://doi.org/10.5281/zenodo.3475642)

This open-source research application provides an application
researchers can use to predict the response of a building subjected to
earthquake events. The application is focused on quantifying the
uncertainties in the predicted response, given the that the properties
of the buildings and the earthquake events are not known exactly, and
that the simulation software and the user make simplifying assumptions
in the numerical modeling of that structure. In the application, the
user is required to characterize the uncertainties in the input. The
application will after utilizing the selected sampling method, provide
information that characterizes the uncertainties in the response
measures. The computations to make these determinations can be
prohibitively expensive. To overcome this impedement the user has the
option to perform the computations on the Stampede2
supercomputer. Stampede2 is located at the Texas Advanced Computing
Center and made available to the user through NHERI DesignSafe, the
cyberinfrastructure provider for the distributed NSF funded Natural
Hazards in Engineering Research Infrastructure (NHERI) facility.

The computations are performed in a workflow application. That is, the
numerical simulations are actually performed by a number of different
applications. The EE-UQ backend software runs these different
applications for the user, taking the outputs from some programs and
providing them as inputs to others. The design of the EE-UQ
application is such that researchers are able to modify the backend
application to utilize their own application in the workflow
computations. This will ensure researchers are not limited to using
the default applications we provide and will be enthused to provide
their own applications for others to use.

This is Version 1.2.0 of the tool and as such is limited in
scope. Researchers are encouraged to comment on what additional
features and applications they would like to see in this
application. If you want it, chances are many of your colleagues also
would benefit from it.

Please visit the [EE-UQ Research Tool webpage](https://simcenter.designsafe-ci.org/research-tools/ee-uq-application/)
for more resources related to this tool. Additionally, this page
provides more information on the NHERI SimCenter, including other SimCenter
applications, FAQ, and how to collaborate.

### Documentation

Tool documentation can be found: https://nheri-simcenter.github.io/EE-UQ-Documentation/


### How to Build

##### 1. Download this repo.

##### 2. Download the SimCenterCommon repo: https://github.com/NHERI-SimCenter/SimCenterCommon

Place the SimCenterCommon Repo in the same directory that you placed the EE-UQ repo.

##### 3. Download the s3hark repo: https://github.com/NHERI-SimCenter/s3hark
Also place this repo in the same directory that you placed the EE-UQ repo.

##### 4. Download the GroundMotionUtilities repo: https://github.com/NHERI-SimCenter/GroundMotionUtilities
Again placing this repo in the same directory that you placed the EE-UQ repo.

##### 5. Install Qt: https://www.qt.io/

Qt is free for open source developers. Download it and start the Qt Creator application. From Qt Creator open the EE-UQ.pro file located in the directory the EE-UQ repo was downloaded into and select build to build it. For detailed instructions on using Qt, browse their website.

#### 6. To run locally you will need to install and build the SimCenterBackendApplications repo: https://github.com/NHERI-SimCenter/SimCenterBackendApplications

SimCenterBackendApplications contains a number of applications written in C++, C and Python. Follow the build instructions on the SimCenterBackendApplications githib page to build them. Once built inside the EE-UQ applicationss preferences set the applications directory entry to point to the applications folder that the build process creates.

### Acknowledgement

This material is based upon work supported by the National Science Foundation under Grant No. 1612843.

### Contact

NHERI-SimCenter nheri-simcenter@berkeley.edu
