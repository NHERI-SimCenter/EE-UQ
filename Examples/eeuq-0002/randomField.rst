.. _lbl-example_randomField:

Including Spatial Variability in Site Response Event
=======================================================

This page shows two examples on how to incorporate spatial variability into UQ analysis by using various special materials, namely ElasticIsotropic,
PM4Sand, and PDMY03, in Site Response option under EVENT tab.

Problem Statement
---------------------------------------------
Site response analysis is commonly performed to analyze the propagation of seismic wave through soil. As shown in :numref:`fig_siteResponse`, 
one-dimensional response analyses, as a simplified method, assume that all boundaries are horizontal and that the response of a soil deposit is
predominately caused by SH-waves propagating vertically from the underlying bedrock. Ground surface response is usually the major output from
these analyses, together with profile plots such as peak horizontal acceleration along the soil profile. When liquefiable soils are presenting,
maximum shear strain and excess pore pressure ratio plots are also important.

In the real world condition, physical properties of soils vary from place to place within a soil deposit due to varying geologic formation and loading histories such as sedimentation, erosion, transportation,
and weathering processes. This spatial variability in the soil properties cannot be simply described by a mean and variance, as commonly adopted in UQ analyses,
since the estimation of the two statistic values does not account for the spatial variation of the soil property data in the soil profile.
Spatial variability is often modeled using two separated components: a known deterministic trend and a residual variability about the trend.
These components are illustrated in :numref:`fig_InherentVariability`.

.. _fig_siteResponse:
.. figure:: ./figures/siteResponse.png
   :scale: 50%
   :alt: Diagram illustrating the concept of soil-structure interaction with a representation of soil over an elastic halfspace. A building foundation is depicted above the soil, with an arrow indicating the bedrock "within" motion. To the right, a vertical cross-section of the foundation shows its interaction with different soil layers, with a red dashed line suggesting a reference level.
   :align: center
   :figclass: align-center

   Simplified 1D site response analysis (courtesy of Pedro Arduino)

.. _fig_InherentVariability:
.. figure:: ./figures/InherentVariability.png
   :scale: 60 %
   :alt: A diagram representing stratified geological layers beneath the ground surface. The image shows several horizontal lines, each labeled as "Layer 1," "Layer i," and "Layer j," indicating different strata. A vertical line labeled "z" has arrows pointing downwards with distances l1 and li marking the depth of the first two layers. In layer j, there is an irregular, wavy line representing the "Deviation from trend, w(z)." This irregular line is superposed on a smooth, undulating line labeled "Trend, t(z)." The diagram also includes annotations for "Scale of Fluctuation, δv" and "Soil Property, ξ(z)", suggesting a focus on the variability of soil properties with depth. The image is likely from a geotechnical or geological educational resource, illustrating concepts such as stratification, soil property variation, and scales of fluctuation within soil layers.
   :align: center
   :figclass: align-center

   Inherent soil variability (after :cite:`Phoon1999`).

A summary of the random field preparation procedure for the site response event analysis is summarized here:

1. Generate mean field using mean target soil property, e.g., relative density (Dr) or shear wave velocity (Vs)
2. Generate Gaussian random field for target soil property using *Gauss1D.py* with mean = 0.0 and :math:`\sigma` = 1.0
3. Interpolate Gaussian field to FEM mesh
4. Combine the mean (trend) field and Gaussian (residual variability) field to obtain a stochastic field
5. Generate material input for site response analysis based on predefined model calibration methods
6. Perform site response analysis using the randomized material input and obtain acceleration response at the surface of soil column for subsequent UQ analysis

.. note::
   - Currently only **2D** plain-strain materials (including PDMY03 and ElasticIsotropic) are supported when using random field. Therefore, 1-component motion is required.

General Workflow for Global Sensitivity Analysis
-----------------------------------------------------------

In a global sensitivity analysis the user is wishing to understand what is the influence of the individual random variables on the quantities of interest.
This is typically done before the user launches large scale forward uncertainty problems in order to limit the number of random variables used so as to limit the number of simulations performed.

..
   For this problem we will limit the response qunataties of interest to the following six quantaties. Peak Roof displacement in 1 and 2 directions,
root mean square (RMS) accelerations in 1 and 2 directions, Peak BAse shear and moments in 1 and 2 directions. 

To perform a Global Sensitivity analysis the user would perform the following steps:

The steps involved:

1. Start the application and the UQ Selection will be highlighted. In the panel for the UQ selection, keep the UQ engine as that selected, i.e. Dakota. From the UQ Method Category drop down menu
select Sensitivity Analysis, Keeping the method as LHS (Latin Hypercube). Change the **# Samples** to 20 and the **Seed** to 273 as shown in the figure. The seed specification allows a user to obtain repeatable
results from multiple runs. 

.. figure:: ./figures/randomField-UQ.png
   :align: center
   :alt: A screenshot of a software interface with a section labeled "UQ Engine" displaying settings for Dakota Method Category with "Forward Propagation" selected. Below, a method "LHS" is chosen with fields for "# Samples" set to 20 and "Seed" set to 273. To the left, a sidebar with abbreviations "UQ," "GI," "SIM," "EVT," "FEM," "EDP," "RV," and "RES" is visible, suggesting sections or modules within the application. The right side of the screen is mostly blank, indicating that additional content might be generated or displayed upon further interaction with the software.
   :figclass: align-center

2. Next select the **GI** panel. In this panel the building properties and units are set. For this example enter **1** for the number of stories, **144** for building height, **360** fow building width,
and **360** for building depth.

.. figure:: ./figures/randomField-GI.png
   :align: center
   :alt: Screenshot of a user interface with a section labeled "Building Information" which includes input fields for the building's name, properties such as the number of stories, height, width, depth, and plan area, and location details including latitude and longitude coordinates. Below that, there is a "Units" section with dropdown menus for selecting the units for force, length, and temperature, set to Kips, Inches, and Celsius respectively. The image shows a comprehensive form for entering and customizing building information for an engineering or architectural software application.
   :figclass: align-center

3. Next select the **SIM** tab from the input panel. This will default in the MDOF model generator. 
Define other input variables as shown in figure:

.. figure:: ./figures/randomField-SIM.png
   :align: center
   :alt: Screenshot of a Building Model Generator software interface with various fields for inputting structural parameters such as number of stories, floor weights, stiffness, and yield strength. The interface is divided into sections labeled 'Building Information', 'Rotational Story Stiffness', 'Mass Eccentricity', and response parameters. The screen is mostly filled with text boxes for data entry, with a scrollbar on the right side and a visual representation of a building model with a single blue square denoting a story or mass.
   :figclass: align-center

3. Next select the **EVT** panel. From the Load Generator pull down menu select the **Site Response** option. Define soil profile, ground water table (GWT), and mesh. Then select interested material, e.g., 
*PM4Sand_Random*, *PDMY03_Random*, or *Elastic_Random*. Under **Configure** tab, select path to the input motion.

.. note::
   - A reasonable mesh resolution is recommended. Selection of element size should consider several factors, including but not limited to, layer shear wave velocity (for frequency resolution), corelation length (for random field resolution), and computation efficiency.

.. figure:: ./figures/randomField-EVT.png
   :align: center
   :alt: Screenshot of an engineering software interface with multiple panels. On the left, there is a vertical navigation menu with options such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES, highlighting the current selection, "UQ." Beside the menu, two graphical representations of soil layers appear, with the left graph labeled "Load Generator" displaying a single red column, and the right graph labeled "Site Response" showing multiple blue layers. In the center and right portions of the screen, there are input fields and dropdown menus for configuring soil layer properties and parameters, such as thickness, density, and material type, with options like "Rock," "PM4Sand_Random," and others. The interface also includes sections for configuring the parameters of the PM4Sand model and the hydraulic properties of the selected layer.
   :figclass: align-center

3. Next choose the **FEM** panel. Here we will change the entries to use Rayleigh damping, with rayleigh factor chosen using **1** mode.

.. figure:: ./figures/randomField-FEM.png
   :align: center
   :alt: Screenshot of a software interface titled 'FE Application' with various parameters and options for an analysis setup. The parameters include Analysis, Integration, Algorithm, ConvergenceTest, Solver, Damping Model, Damping Ratio, Selected Tangent Stiffness, and Mode specifications, with text boxes and dropdown menus for user input. A 'Choose' button is located beneath the 'Analysis Script' field. The interface has a sidebar with various options such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES, possibly indicating different modules or sections within the software.
   :figclass: align-center

4. We will skip the **EDP** panel leaving it in it's default condition, that being to use the **Standard Earthquake** EDP generator.

.. figure:: ./figures/randomField-EDP.png
   :align: center
   :alt: Screenshot of a user interface with a navigation menu on the left side, featuring various tabs such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES. The main area of the interface has a title bar with the text "Engineering Demand Parameter Generator" and a dropdown menu option labeled "Standard Earthquake." The rest of the screen is mostly blank, indicating either a minimalist design or that content has not been loaded or is not visible in this view.
   :figclass: align-center

5. For the **RV** panel, we will enter the distributions and values for our random variables. If only the uncertainty related to spatial variability is interested, a dummy random variable can be defined in this tab.
Then all the variability shown in the response will solely be due to spatial variability in the site response analysis. 

.. figure:: ./figures/randomField-RV.png
   :align: center
   :alt: Screenshot of a software interface with a sidebar on the left featuring options such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES in a vertical list, with the RV option highlighted. The main part of the interface has a heading titled "Input Random Variables" followed by an Add and Remove button and a Correlation Matrix button. Below there is a table with columns for Variable Name, Distribution, Mean, and Standard Dev; a single row contains the variable name 'dummy,' a dropdown indicating 'Normal' distribution selected, a mean of 10, and a standard deviation of 1. There is also a Show PDF button located to the right of the table. The interface has a minimalist design with a color scheme of blues and grays.
   :figclass: align-center

.. warning::   

   The user cannot leave any of the distributions for these values as constant for the Dakota UQ engine.

5. Next click on the 'Run' button. This will cause the backend application to launch dakota. When done the **RES** tab will be selected and the results will be displayed. The results show the values the mean and standard deviation. 
The peak displacement of the roof, is the quantity **PFD**. The **PFA** and **PFD** quantity defines peak floor acceleration and displacement, respectively, and the **PID** quantity corresponds to peak interstory drift.

.. figure:: ./figures/Elastic-RES.png
   :align: center
   :alt: A screenshot of a statistical analysis software interface displaying a table of summary statistics for different data sets. The columns include 'Name', 'Mean', 'StdDev' (standard deviation), 'Skewness', and 'Kurtosis'. There are five sets of data labeled from '1-PFA-0-1' to '1-PRD-1-1' with respective mean values ranging from 139.483 to 0.0326987, standard deviations ranging from 12.2984 to 0.00502987, skewness fixed at 2.52653 for four datasets, and kurtosis fixed at 10.0967 also for four datasets. On the left side, tabs labeled 'UQ', 'GI', 'SIM', 'EVT', 'FEM', 'EDP', 'RV', and a selected 'RES' tab are visible, indicating different modules or sections within the software.
   :figclass: align-center


Adding Spatial Variability
-----------------------------------------------------------

Case 1: using ElasticIsotropic material
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For the **Elastic_Random** material, shear wave velocity (Vs) can be selected to be randomized. Then select the **Mean** and **COV** (coefficient of variation :math:`=\frac{\sigma}{\mu}`) for shear wave velocity.  
**Correlation length** defines how shear wave velocities are vertically correlated. Subsequently, Young's modulus is calculated based the stochastic shear velocity profile at the center of each element. No special calibration is required.

.. note::
   - Vs is bounded between 50 and 1500 m/s. These limits can be modified in *calibration.py*.

.. figure:: ./figures/Elastic-Random.png
   :scale: 60 %
   :alt: Screenshot of a software interface for geotechnical analysis. The left side, labeled "Load Generator," displays two vertical bars representing layers of soil or rock with a green arrow indicating direction. The right side, labeled "Site Response," contains input fields and tables with parameters for geotechnical properties, like "Layer Name," "Thickness," "Density," "Vs," "Material," and "Element Size." A section below contains fields for "Elastic modulus," "Poisson's ratio," and "Stochastic field parameters," including "Mean," "COV," and "Correlation Length," reflecting settings for an ElasticIsotropic model. The interface is designed for configuring and analyzing the response of geological layers to load.
   :align: center
   :figclass: align-center

   Define inputs for Elastic_Random material.

:numref:`fig_Elastic-Average-RespSpect` presents the profiles of shear wave velocity, peak horizontal acceleration, maximum shear strain, and maximum excess pore pressure ratio (Ru) obtained from 20 realizations.
Ru are always zero since there is no volumetric strain in ElasticIsotropic material. :numref:`fig_Elastic-Average-RespSpect` depicts the mean and each individual response spectra (5% damping) at surface obtained from 20 realizations.

.. _fig_Elastic-Average-Profile:
.. figure:: ./figures/Elastic-Average-Profile.png
   :scale: 40 %
   :alt: The image displays a series of four vertical graphs side by side, plotting various geotechnical parameters against depth in meters. The first graph shows numerous wavy, overlapping lines representing shear wave velocity, with values ranging from 0 to 300 meters per second. The second graph depicts peak horizontal acceleration (PHA) with a thick dark line and multiple thinner lines around it. The third graph illustrates maximum shear strain percentage, with a similar pattern of a central bold line and lighter lines. The last graph shows maximum Ru, with individual realizations depicted as thin lines and the average as a bold line, though this graph has significantly fewer data points compared to the others. Each graph has a y-axis labeled "Depth (m)" ranging from 0 to 6 meters, and x-axes labeled with their respective parameters. The color scheme is primarily shades of blue and black.
   :align: center
   :figclass: align-center

   Profiles of shear wave velocity, peak horizontal acceleration, maximum shear strain, and maximum excess pore pressure ratio (Ru) obtained from 20 realizations.

.. _fig_Elastic-Average-RespSpect:
.. figure:: ./figures/Elastic-Average-RespSpect.png
   :scale: 20 %
   :alt: A graph displaying spectral acceleration (S_a in g) on the y-axis and period (sec) on the x-axis on logarithmic scales. Multiple light blue lines represent individual realizations of a dataset, fluctuating around a prominent, bold black line that indicates the average value. The lines show increased spectral acceleration at certain periods, with a peak roughly between 0.1 to 1 second, before tapering off at longer periods. A legend on the top right labels the black line as "Average" and the blue lines as "Individual Realization."
   :align: center
   :figclass: align-center

   Response spectra (5% damping) at surface obtained from 20 realizations.



Case 2: using PM4Sand material
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
For the **PM4Sand_Random** material, relative density (Dr) can be selected to be randomized. Then select the **Mean** and **COV** (coefficient of variation :math:`COV=\frac{\sigma}{\mu}`) for shear wave velocity.  
**Correlation length** defines how shear wave velocities are vertically correlated. In ths current calibration procedure, all the other parameters are kept as input except for the contraction rate
parameter hpo, that is calibrated based on the empirical triggering model proposed by Idriss and Boulanger 2008.

:numref:`fig_PM4Sand-Average-RespSpect` presents the profiles of shear wave velocity, peak horizontal acceleration, maximum shear strain, and maximum excess pore pressure ratio (Ru) obtained from 20 realizations.
Comparing to elastic material, more variability are shown among these realizations. :numref:`fig_PM4Sand-Average-RespSpect` depicts the mean and each individual response spectra (5% damping) at surface obtained from 20 realizations.

.. figure:: ./figures/PM4Sand-Random.png
   :scale: 60 %
   :alt: "Screenshot of a geotechnical engineering software interface displaying options for load generation and site response analysis. The left side shows a sidebar with various module tabs like UQ, GI, SIM, EVT, FEM, EDP, RV, and RES; it also has a vertical graphical representation of a soil profile with two layers, the top one being blue and labelled 'Rock.' The main window is divided into sections, including 'Layer properties' and 'Response.' User-entered parameters are visible for a PM4Sand model, with input fields for properties like Density, Vs, Material, and ElementSize, along with more detailed parameters such as Dr, Go, and Den. Various other inputs for model configuration like Mean, COV, and Correlation Length are outlined in red. The bottom right corner indicates a 0% progress bar, suggesting that an analysis is yet to be run."
   :align: center
   :figclass: align-center

   Define inputs for PM4Sand_Random material.

.. note::
   - Dr is bounded between 0.2 and 0.95. These limits can be modified in *calibration.py*.

.. _fig_PM4Sand-Average-Profile:
.. figure:: ./figures/PM4Sand-Average-Profile.png
   :scale: 40 %
   :alt: The image displays a collection of four line graphs, arranged side by side, each graph plotting multiple lines with a common Y-axis labeled 'Depth (m)' ranging from 0 to 6 meters. Each graph represents different geotechnical parameters against depth. The first graph is captioned 'Relative Density,' with lines ranging values between 0.00 and 0.75. The second graph is titled 'PHA(g),' with lines varying between 0.0 and 0.4 g. The third graph is marked 'Max Shear Strain (%)' with lines spreading from 0 to over 20 percent. The last graph is labeled 'Max Ru,' with lines extending from 0.00 to 1.00. In all graphs, multiple light blue lines represent individual realizations, and one bold black line indicates the average for each parameter. The plots highlight the variability of these parameters with depth, suggesting a probabilistic analysis.
   :align: center
   :figclass: align-center

   Profiles of shear wave velocity, peak horizontal acceleration, maximum shear strain, and maximum excess pore pressure ratio (Ru) obtained from 20 realizations.


.. _fig_PM4Sand-Average-RespSpect:
.. figure:: ./figures/PM4Sand-Average-RespSpect.png
   :scale: 20 %
   :alt: Image showing error in description
   :align: center
   :figclass: align-center

   Response spectra (5% damping) at surface obtained from 20 realizations.






