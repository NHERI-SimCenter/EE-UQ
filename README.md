# EE-UQ

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3475642.svg)](https://doi.org/10.5281/zenodo.3475642)

[![Build status](https://ci.appveyor.com/api/projects/status/kk9ik9v0nkx8x1e8?svg=true)](https://ci.appveyor.com/project/fmckenna/ee-uq)

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

This tool is under active development. Researchers are encouraged to comment on what additional
features and applications they would like to see in this
application. If you want it, chances are many of your colleagues also
would benefit from it. Additional requests should be posted at the [forum](https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=6.0)

Please visit the [EE-UQ Research Tool webpage](https://simcenter.designsafe-ci.org/research-tools/ee-uq-application/)
for more resources related to this tool. Additionally, this page
provides more information on the NHERI SimCenter, including other SimCenter
applications, FAQ, and how to collaborate.

### Documentation

Tool documentation can be found: https://nheri-simcenter.github.io/EE-UQ-Documentation/


### Acknowledgement

This material is based upon work supported by the National Science Foundation under Grant No. 1612843.

### Contact

NHERI-SimCenter nheri-simcenter@berkeley.edu
