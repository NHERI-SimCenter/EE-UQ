#

<div style="display: flex; justify-content: center;">
  <a href="https://github.com/NHERI-SimCenter/EE-UQ/#gh-light-mode-only"><img width=256px src="./icons/NHERI-EEUQ-Icon_LightMode_BorderRight.svg#gh-light-mode-only" align="left" /></a>
  <a href="https://github.com/NHERI-SimCenter/EE-UQ/#gh-dark-mode-only"><img width=256px src="./icons/NHERI-EEUQ-Icon_DarkMode_BorderRight.svg#gh-dark-mode-only" align="left" /></a>
  <span style="display:inline-block; width: 25px;"></span>
  <div>
    <p>
      <h3 class="subtitle"><b>EE-UQ - Desktop App</b></h3>
      <h3>Earthquake Engineering with Uncertainty Quantification</h3>
      <h5><i>Frank McKenna, Kuanshi Zhong, Michael Gardner, Adam Zsarnoczay, Sang-ri Yi, Aakash Bangalore Satish, Charles Wang, Wael Elhaddad & Peter Mackenzie-Helnwein</i></h5>
      <h5>NHERI SimCenter, 2017-2025</h5>
      <br>
    </p>
  </div>
</div>

---

[![Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/EE-UQ?color=blue&label=Latest%20Release)](https://github.com/NHERI-SimCenter/EE-UQ/releases/latest)   <span style="display:inline-block; width: 20px;"></span> [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1439079.svg)](https://doi.org/10.5281/zenodo.1439079)   <span style="display:inline-block; width: 20px;"></span> [![Build status](https://ci.appveyor.com/api/projects/status/k1cfrfmjsq14akso?svg=true)](https://ci.appveyor.com/project/fmckenna/ee-uq)  <span style="display:inline-block; width: 20px;"></span> [![License](https://img.shields.io/badge/License-BSD%202--Clause-blue)](https://raw.githubusercontent.com/NHERI-SimCenter/EE-UQ/master/LICENSE)  <span style="display:inline-block; width: 20px;"></span> [![GitHub](https://img.shields.io/badge/NHERI--SimCenter-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter)  <span style="display:inline-block; width: 20px;"></span>  [![LinkedIn Follow](https://img.shields.io/badge/nheri--simcenter-%230077B5.svg?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/company/nheri-simcenter) <span style="display:inline-block; width: 20px;"></span>  [![YouTube Subscribe](https://img.shields.io/badge/DesignSafe-%23FF0000.svg?style=for-the-badge&logo=YouTube&logoColor=white)](https://www.youtube.com/@DesignSafe) <span style="display:inline-block; width: 20px;"></span>  

---

# Why use EE-UQ

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1439079.svg)](https://doi.org/10.5281/zenodo.1439079)

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
option to perform the computations on the Stampede3
supercomputer. Stampede3 is located at the Texas Advanced Computing
Center and made available to the user through NHERI DesignSafe, the
cyberinfrastructure provider for the distributed NSF funded Natural
Hazards in Engineering Research Infrastructure (NHERI) facility.

The computations are performed in a workflow application. That is, the
numerical simulations are actually performed by a number of different
applications. The ``EE-UQ`` backend software runs these different
applications for the user, taking the outputs from some programs and
providing them as inputs to others. The design of the ``EE-UQ``
application is such that researchers are able to modify the backend
application to utilize their own application in the workflow
computations. This will ensure researchers are not limited to using
the default applications we provide and will be enthused to provide
their own applications for others to use.

This tool is under active development. Researchers are encouraged to comment on what additional
features and applications they would like to see in this
application. If you want it, chances are many of your colleagues also
would benefit from it. Additional requests should be posted at the [SimCenter forum](https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=6.0).

Please visit the [EE-UQ Research Tool webpage](https://simcenter.designsafe-ci.org/research-tools/ee-uq-application/)
for more resources related to this tool. Additionally, this page
provides more information on the NHERI SimCenter, including other SimCenter
applications, FAQ, and how to collaborate.

### Documentation

Tool documentation can be found at the [EE-UQ Documentation Page](https://nheri-simcenter.github.io/EE-UQ-Documentation/)


### Acknowledgement

This material is based upon work supported by the National Science Foundation under grants #1612843 and #2131111

### Contact

NHERI-SimCenter nheri-simcenter@berkeley.edu
