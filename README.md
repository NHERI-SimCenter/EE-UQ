[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2619616.svg)](https://doi.org/10.5281/zenodo.2619616)

# EE-UQ

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

This is Version 1.1.0 of the tool and as such is limited in
scope. Researchers are encouraged to comment on what additional
features and applications they would like to see in this
application. If you want it, chances are many of your colleagues also
would benefit from it.

## How to Cite
You can cite this software as follows:

Frank McKenna, Wael Elhaddad, Michael Gardner, & Charles Wang. (2019, March 31). NHERI-SimCenter/EE-UQ v1.1.0 (Version v1.1.0). Zenodo. http://doi.org/10.5281/zenodo.2619616

## Acknowledgements
This material is based upon work supported by the National Science Foundation under Grant No. 1612843.