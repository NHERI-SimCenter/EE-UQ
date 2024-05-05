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



## Quick Links

- [Download Application](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/EE_UQ)
- [Step-by-Step Examples](https://nheri-simcenter.github.io/EE-UQ-Documentation/common/user_manual/examples/desktop/examples)
- [Documentation & Guides](https://nheri-simcenter.github.io/EE-UQ-Documentation)
- [Overview Web-Page](https://simcenter.designsafe-ci.org/research-tools/ee-uq)
- [Forum & Feature Requests](http://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=17.0)


## Citing this Work

If you use ``EE-UQ`` [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.10902075.svg)](https://doi.org/10.5281/zenodo.10902075) in your research, please cite our software as:

```bibtex
@software{McKennaZhongGardnerZsarnoczayYiSatishWangElhaddad2024EEUQ,
  author       = {Frank McKenna and
                  Kuanshi Zhong and
                  Michael Gardner and
                  Adam Zsarnoczay and
                  Sang-ri Yi and
                  Aakash Bangalore Satish and
                  Charles Wang and
                  Wael Elhaddad},
  title        = {NHERI-SimCenter/EE-UQ: Version 3.5.0},
  month        = apr,
  year         = 2024,
  publisher    = {Zenodo},
  version      = {v3.5.0},
  doi          = {10.5281/zenodo.10902075},
  url          = {https://doi.org/10.5281/zenodo.10902075}
}
```

and include the NHERI SimCenter's workflow architecture [![DOI](https://img.shields.io/badge/10.3389--fbuil.2020.558706-blue?label=DOI)](https://www.frontiersin.org/articles/10.3389/fbuil.2020.558706) using:

```bibtex
@Article{Deierlein2020,
  author={Deierlein, Gregory G. and McKenna, Frank and Zsarnóczay, Adam and Kijewski-Correa, Tracy and Kareem, Ahsan and Elhaddad, Wael and Lowes, Laura and Schoettler, Matthew J. and Govindjee, Sanjay},   
  title={A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment},      
  journal={Frontiers in Built Environment},      
  volume={6},           
  year={2020},      
  url={https://www.frontiersin.org/articles/10.3389/fbuil.2020.558706},       
  doi={10.3389/fbuil.2020.558706},      
  issn={2297-3362},   
}
```

---

## SimCenter Eco-System

The challenges of natural hazards engineering are addressed by the NHERI SimCenter through a suite of applications that provide cutting-edge tools for researchers, practitioners, and stakeholders. The applications are designed to work together to provide a comprehensive solution for natural hazards engineering. A puzzle-piece diagram of the SimCenter ecosystem is shown below:



<a href="https://github.com/NHERI-SimCenter/"><img width="75%" src="./icons/SimCenter_PuzzlePieces_Horizontal.png" align="center" /></a>



In reality, this is a software workflow representation of the PEER Performance-Based Earthquake Engineering (PBEE) framework that has been extended to include other natural hazards:



<a href="https://github.com/NHERI-SimCenter/"><img width="85%" src="./icons/SimCenter_PEER_PBE.png" align="center" /></a>



``EE-UQ`` is just one part of the [NHERI SimCenter](https://simcenter.designsafe-ci.org) ecosystem that provides cutting-edge open-source tools for natural hazards engineering. Tools like ``quoFEM``, ``EE-UQ``, ``WE-UQ``, ``HydroUQ``, ``PBE``, and ``R2D`` work together to provide a comprehensive solution for natural hazards engineering. The SimCenter ecosytem forms a modular hierarchy that allows users to pick and choose the tools they need for their specific research or engineering problem.



<img width=60% align="left" src="https://raw.githubusercontent.com/JustinBonus/EE-UQ/master/icons/SimCenter_Hierarchy.png" />

While ``R2D`` is the top-level application that provides a regional resilience assessment, ``EE-UQ``, ``WE-UQ``, and ``HydroUQ`` provide uncertainty quantified simulations for earthquake, wind, and water-borne natural hazards, respectively. ``quoFEM`` is the backend UQ functionality they use.

Additional tools, such as ``BRAILS`` and ``TInF``, have special use-cases including AI-augmentation of building stock and creation of turbulent wind inflow for OpenFOAM CFD simulations.


All applications are free, open-source, and available for download on the [DesignSafe-CI](https://www.designsafe-ci.org) website. See the table below for more information on each application:



| Application | Full Title | Download | GitHub | Version |
|:-----------:|:-----------|:--------:|:------:|:-------:|
| ``quoFEM`` | Quantified Uncertainty with Optimization for the Finite Element Method Application | [![Download quoFEM](https://img.shields.io/badge/Download-quoFEM-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/quoFEM) | [![GitHub quoFEM](https://img.shields.io/badge/git%20quoFEM-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/quoFEM) |  [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/quoFEM)](https://github.com/NHERI-SimCenter/quoFEM/releases/latest) |
| ``EE-UQ`` | Earthquake Engineering with Uncertainty Quantification Application | [![Download EE-UQ](https://img.shields.io/badge/Download-EE--UQ-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/EE_UQ) | [![GitHub EE-UQ](https://img.shields.io/badge/git%20EE--UQ-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/EE-UQ) | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/EE-UQ)](https://github.com/NHERI-SimCenter/EE-UQ/releases/latest) |
| ``WE-UQ`` | Wind Engineering with Uncertainty Quantification Application | [![Download WE-UQ](https://img.shields.io/badge/Download-WE--UQ-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/WE_UQ) | [![GitHub WE-UQ](https://img.shields.io/badge/git%20WE--UQ-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/WE-UQ)  | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/WE-UQ)](https://github.com/NHERI-SimCenter/WE-UQ/releases/latest) |
| ``HydroUQ`` | Water-Borne Natural Hazards Engineering with Uncertainty Quantification Application | [![Download HydroUQ](https://img.shields.io/badge/Download-HydroUQ-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/HydroUQ) | [![GitHub HydroUQ](https://img.shields.io/badge/git%20HydroUQ-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/HydroUQ)  | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/HydroUQ)](https://github.com/NHERI-SimCenter/HydroUQ/releases/latest) |
| ``PBE`` | Performance-Based Engineering Application | [![Download PBE](https://img.shields.io/badge/Download-PBE-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/PBE) | [![GitHub PBE](https://img.shields.io/badge/git%20PBE-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/PBE) | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/PBE)](https://github.com/NHERI-SimCenter/PBE/releases/latest) |
| ``R2D`` | Regional Resilience Determination Application | [![Download R2D](https://img.shields.io/badge/Download-R2D-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/R2Dt) | [![GitHub R2D](https://img.shields.io/badge/git%20R2D-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/R2DTool) | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/R2DTool)](https://github.com/NHERI-SimCenter/R2DTpp;/releases/latest) |
| ``BRAILS`` | Building Recognition with AI at Large-Scale Application | ``pip install BRAILS`` | [![GitHub BRAILS](https://img.shields.io/badge/git%20BRAILS-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/BRAILS) | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/BRAILS)](https://github.com/NHERI-SimCenter/BRAILS/releases/latest) |
| ``TInF`` | Turbulence Inflow Generation for CFD Application | [![Download TInF](https://img.shields.io/badge/Download-TInF-blue)](https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community//SimCenter/Software/TurbulenceInflowTool) | [![GitHub TInF](https://img.shields.io/badge/git%20TInF-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/NHERI-SimCenter/TurbulenceInflowTool) | [![GitHub Latest Release](https://img.shields.io/github/v/release/NHERI-SimCenter/TurbulenceInflowTool)](https://github.com/NHERI-SimCenter/TurbulenceInflowTool/releases/latest) |

---



### Documentation

Tool documentation can be found at the [EE-UQ Documentation Page](https://nheri-simcenter.github.io/EE-UQ-Documentation/)


### Acknowledgement

This material is based upon work supported by the National Science Foundation under grants #1612843 and #2131111

### Contact

NHERI-SimCenter nheri-simcenter@berkeley.edu
