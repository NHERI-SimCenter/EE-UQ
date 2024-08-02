.. _eeuq-0009:


Training GP Surrogate Model
===========================================================================================================

.. warning:: To reproduce the result of this example, the user should first click **EVT** and **Select Records**, and then click the **RUN** button. See the below procedure for details.

Training Gaussian Process (GP) Surrogate Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example shows how to train a surrogate model that can be later imported into EE-UQ and replace structural dynamic simulations. The training of a surrogate model requires some number of structural dynamic simulations for representative scenarios [i.e. training set]. In this example, these simulations are performed using recorded ground motions selected from the PEER NGA database. The ground motions are selected such that they cover a wide domain of intensity measure (IM) space, as uniform as possible. Three IMs are considered in the example: pseudo-spectral acceleration at period 0.5 sec (PSA), 5-75% duration (D5-75), and SaRatio following the work in [Zhong2023]_. We additionally consider the stiffness of the target structure (3-story building) as input for the surrogate model.

      .. figure:: figures/EE09_main3.png
         :name: UQ inputs
         :alt: The image showcases a variety of data visualization graphics against a black background. On the left, there are four 2D plots including a normal distribution curve, three scatter plots with varying densities and sizes of dots, and an audio waveform with one point highlighted in red. On the right, a 3D plot features a surface graph with peaks in a grid pattern, superimposed with red dots at various heights and corresponding to the peaks, and transparent bell curves extending from the surface. The axes suggest the plot may represent some multivariate data.
         :align: center
         :width: 40%
         :figclass: align-center

         Training a surrogate model for seismic response

.. [Zhong2023]
   Zhong, K., Navarro, J.G., Govindjee, S., and Deierlein, G.G., "Surrogate Modeling of Structural Seismic Response Using Probabilistic Learning on Manifolds," *Earthquake Engineering & Structural Dynamics*, 1-22, (2023) https://doi.org/10.1002/eqe.3839

Set Up GP Surrogate Modeling Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In the **UQ tab** select the **Surrogate Modeling** as **UQ Method**. Then, select the **Train GP Surrogate Model**.

      .. figure:: figures/EE09_UQ.png
         :name: UQ inputs
         :alt: Screenshot of a software interface for uncertainty quantification using surrogate modeling. The UQ Engine selected is SimCenterUQ, and the option to Train GP Surrogate Model is chosen. There are options for setting the number of samples, computation time, target accuracy, random seed, and allowing for parallel execution. Advanced options for a Gaussian Process Model are available, as well as earthquake-specific advanced options, including input postprocess for ground motion intensity and a section for adding intensity measure calculations with options such as significant duration and pseudo spectral acceleration.
         :align: center
         :width: 100%
         :figclass: align-center

         UQ tab

   Below are the details of the selected inputs:

   - **Number of Samples** : The number of samples should be the same as the ground motions to be selected in the EVT tab, i.e. 100. This number can be flexible when stochastic ground motion is used. Typically, as the number of samples increases, the accuracy of the surrogate model will increase but training time will also increase.
   - **Max Computation Time** : 60 mins (default)
   - **Target Accuracy** : 0.02. EE-UQ will give a warning when this accuracy target is not met
   - **Random Seed** : 984. An arbitrarily selected seed for the random generator used in the Surrogate modeling algorithm
   - **Parallel Execution** : True. This will parallelize multiple dynamic simulations
   - **Advanced Options for Gaussian Process Model** : False. By default, EE-UQ takes **Martern 5/2**, **Log-space transform of QoI**, **Heteroskedastic nugget variance** options.
   - **Existing Data set** : False. This is useful when one hopes to resume the surrogate model training after one training round is finished, by performing more simulations
   - **Intensity Measure Calculation**: Select the intensity measures (IMs) to be used as the augmented input of the surrogate model -Sa(T=0.5 sec), D5-75, and SaRatio(T=1.0 sec in the range of 0.1-1.5 sec)

More information can be found in the :ref:`User Guide<lblSimSurrogate>`.


Define Target Structure
^^^^^^^^^^^^^^^^^^^^^^^

2. In **GI tab**, set the number of stories, 3.


      .. figure:: figures/EE09_GI.png
         :name: UQ inputs
         :alt: Screenshot of a building information interface with various input fields for data. It includes sections for "Building Information" with an empty 'Name' field, "Properties" listing year built as 1990, number of stories as 3, structural type as RM1, and dimensions in height, width, depth, and plan area. The "Location" section provides latitude and longitude coordinates, and there is a "Units" section to select force, length, and temperature units like Kips, inches, and Celsius. On the left side of the interface, there are navigation menu options such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES.
         :align: center
         :width: 100%
         :figclass: align-center

         GI tab - Specify 3-story building


3. In **SIM tab**, the specifics of the target structural model is provided via **MDOF** building generator. A three-story building is created having stiffness as an input parameter.

      .. figure:: figures/EE09_SIM.png
         :name: UQ inputs
         :alt: Screenshot of a Building Model Generator software interface, with an MDOF (Multiple Degrees of Freedom) tab selected. The interface displays input fields for building information such as Number Stories, Floor Weights, and Story Stiffness among others, complete with values and parameters for X and Y directions. It includes options for defining characteristics like Damping Ratio, Hardening Ratio, and Response Eccentricity. Additional parameters including Weight, Height, and stiffness and yield strength coefficients for X and Y directions are entered into a table format at the bottom.
         :align: center
         :width: 100%
         :figclass: align-center

         SIM tab

Select Ground Motions for the Training
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

4. In **EVT tab**, the option to retrieve ground motions from **PEER NGA records** is selected. To allow the surrogate models to cover a wide variety of ground motions (represented as "wide IM domain"), let us select **No Spectrum - Uniform IMs** for the analysis. Set the **Number of samples per bin** 1, and add three intensity measures that are specified in the UQ tab as the input of surrogate, i.e. "5-75% Significant Duration", "Pseudo Spectral Acceleration (with a natural period of 0.5 sec)", and "SaRatio (at 1.0 sec in range of 0.1-1.5 sec). Set the coverage ranges respectively to [2.5, 30], [0.1, 2.0], and [0.25, 1.2]. For the former two IMs, the number of bins is set as 5, and the SaRatio will have 4 bins. Therefore, the total number of bins (i.e. grid points in 3-dimensional space) is 100. 

      .. figure:: figures/EE09_EVT1.png
         :name: UQ inputs
         :alt: Screenshot of a Load Generator interface from a software application with sections for Target Spectrum and Intensity Measure Calculation. Options include selecting types of spectra, defining number of samples per bin, and intensity measure parameters such as significant duration, pseudo spectral acceleration, and SaRatio with input fields for periods, minimum, maximum values, and number of bins. A note at the bottom indicates that the number of ground motions to be selected is 100. There is also a path to a temporary records directory shown at the bottom right, indicating a local user directory on a Windows PC.
         :align: center
         :width: 100%
         :figclass: align-center

         EVT tab - configuration of IM bins

 The selected excitation time histories will be saved in the "Temporary records Directory" shown in the figure. It is recommended to use a user-defined directory to reuse the data files in different analyses.

 .. warning::   

   Due to copyright issues, PEER imposes a strict limit on the number of records that can be downloaded within a unique time window. The current limit is set at approximately 200 records every two weeks, 400 every month. Please make sure this limit is not exceeded. Otherwise, the analysis will fail.

* **Temporary Records Directory** is where the downloaded ground motion records are stored. 
* **Acceleration Components** option is used to select the directional components to be used in the analysis. For example, if H1 is selected, single-directional ground motions will be excited to the structure.

      .. figure:: figures/EE09_EVT2.png
         :name: UQ inputs
         :alt: Screenshot of a software interface displaying a table titled "Ground Motion Components" with columns for RSN, Scale, Earthquake, Station, Magnitude, Distance, and Vs30, containing data on various earthquakes and their attributes. An option menu titled "Acceleration Components" is set to "Suite Average" with dropdowns for SRSS and Geometric. Below the table, a section labeled "Scaling/Selection Criteria" with "Scaling Method:" followed by a dropdown menu set to "No Scaling" and a blue "Select Records" button.
         :align: center
         :width: 70%
         :figclass: align-center

         EVT tab - Selected ground motions in a table
 

.. |PEER Ground Motion Database| raw:: html

    <a href="https://ngawest2.berkeley.edu/" target="_blank">PEER Ground Motion Database</a>

* Press **Select Records** when ready, which will connect the PEER NGA West Ground Motion Database. You could use your account and password to log in. If you don’t have an account, you can easily sign up at |PEER Ground Motion Database|. The list of selected ground motions and their scaling factors are displayed in the table. The coverage of IMs of the selected ground motions will be displayed in the right-hand side panel as below.

      .. figure:: figures/EE09_EVT3.png
         :name: UQ inputs
         :alt: This is a 3D scatter plot displaying a range of data points classified by color to indicate ground motion coverage error levels. The axes are labeled as PSA(g) for the vertical axis, SaRatio for the depth axis, and DS575(sec) for the horizontal axis. Data points are differentiated by color intensity, from red through purple to blue, with a color scale legend on the right. Points are also categorized as either "anchor point" indicated by a solid black dot or "selected ground motion" represented by a hollow circle. The plot visually represents the relationship between these three variables in the context of ground motion coverage with varying error levels.
         :align: center
         :width: 500
         :figclass: align-center

         EVT tab - Selected ground motions in IM space

Note that *approximated* IM values are used for this ground motion selection. The approximated IMs are read from the flat file PEER provides, and the geometric mean is used to average out the two horizontal directional components. Therefore the actual IM used as the surrogate input may not exactly match the IM value shown in the above figure. The yellow dots represent the selected 100 ground motion records having corresponding IMs, blue/red dots represent the center of each bin, i.e. anchor point. It is colored red when no matching ground motion is found to be close to the anchor point. This informs users how good the IM coverage is.

.. warning::  Note that the surrogate modeling algorithms are stronger in "interpolation" rather than extrapolation. Therefore, when later using the pre-trained surrogate model to predict the response of the structure subjected to new (untrained) IM values (e.g. :ref:`example 10<eeuq-0010>`), it is important to make sure the IMs of the new ground motions are well covered by the domain of the training samples, i.e. it should lie the area that is shown blue in the above figure. Otherwise, the prediction from the surrogate model is likely not reliable.

5. The **FEM tab** is kept as default.

.. warning::   

   Do NOT select the "None (only for the surrogate)" option in the FEM tab. This option is not for training a surrogate model but for using a pre-trained surrogate model. (See :ref:`example 10<eeuq-0010>`)

6. The **EDP tab** is kept as default. For the surrogate model to be compatible with the PBE and other applications, it should follow the naming of the Standard Earthquake. Under the **Standard Earthquake**, in this example, the structural model will automatically output peak floor acceleration (PFA), peak floor displacement respective to the ground (PFD), Peak inter-story drift ratio (PID), peak roof drift ratio (PRD). 

.. warning::   

   Do NOT select the "None (only for the surrogate)" option in the EDP tab. This option is not for training a surrogate model but for using a pre-trained surrogate model. (See :ref:`example 10<eeuq-0010>`)

Set Up Training Domain
^^^^^^^^^^^^^^^^^^^^^^

7. In **RV tab** set the range of stiffness to be [50, 150] as shown in the below image. This is equivalent to the range of stiffness of which the response can be predicted using the surrogate. The selected IMs (Sa, D5-75, and SaRatio) in each two horizontal directions will be additional inputs of the surrogate model. Therefore, the total dimension of the surrogate model input is 7.

      .. figure:: figures/EE09_RV.png
         :name: UQ inputs
         :alt: Screenshot of a user interface for inputting random variables, including fields for naming the variable and selecting its distribution type, which in this case is set to 'Uniform' with a minimum of 50 and maximum of 150. Buttons for "Add", "Clear All", "Correlation Matrix", "Show PDF", "Export", and "Import" are displayed, accompanying three tabs on the left side labeled "UQ", "GI", and "SIM".
         :align: center
         :width: 100%
         :figclass: align-center

         RV tab


Run the Analysis and Process Results
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

8. Click **Run** button. The analysis may take several minutes to run. The **RES tab** will be highlighted when the analysis is completed.

   The EDP name consists of the quantity of interest, story number, and the direction of interest - for example:

      * 1-PFA-0-1 : **peak floor acceleration** at the **ground floor**, **component 1** (x-dir)
      * 1-PFD-1-2 : **peak floor displacement** (respective to the ground) at the **1st floor** ceiling, **component 2** (y-dir)
      * 1-PID-3-1 : **peak inter-story drift ratio** of the **3rd floor**, **component 1** (x-dir)   
      * 1-PRD-1-1 : **peak roof drift ratio**, **component 1** (x-dir)   

9. Two Goodness-of-fit measures are provided : Inter-quartile ratio (IQR) and normality (Cramer-Von Mises test) score. Using the leave-one-out cross-validation predictions, the IQR provides the ratio of the sample QoIs that lies in 25-75% LOOCV prediction bounds (interquartile range). The IQR values should theoretically approach 0.5 if the prediction is accurate.

   .. figure:: figures/EE09_RES1.png
         :name: UQ inputs
         :alt: A screenshot showing a summary of a surrogate modeling process with the message "Surrogate Modeling Completed! - Process ended as the maximum allowable number of simulations is reached." The table includes details like the number of training samples (100), model simulations (100), and analysis time (17.3 min). It also displays a Goodness-of-Fit section with two metrics: "Inter-quartile ratio" and "Normality (Cramér-von Mises) test" for different model configurations, with values ranging from 0.510 to 0.710 for the inter-quartile ratio and from 0.031 to 0.988 for normality. One inter-quartile ratio value for the configuration '1-PFA-2-2' is highlighted in red at 0.600, and there is a note below stating that some or all of the QoIs have an inter-quartile ratio far from the target value (IQRatio=0.5).
         :align: center
         :width: 100%
         :figclass: align-center

         RES tab - summary

   

10. Additionally the comparison between the original simulation samples and leave-one-out cross-validation (LOOCV) predictions from the surrogate is provided as the scatter plot. When we have a large variance in the response observations, as in this example, the LOOCV mean is not expected to match exactly the data samples observed. Instead, they are expected to lie on a reasonable prediction bound. The below graph shows the inter-quartile prediction bounds, i.e. 25%-75%, which are expected to cover 50% of the sample observations. 

      .. figure:: figures/EE09_RES2.png
         :name: UQ inputs
         :alt: An image depicting a scatter plot titled "Leave-One-Out Cross-Validation (LOOCV) Prediction." The plot shows individual data points along with vertical error bars indicating a 50% prediction interval. The y-axis is labeled "Training sample" and the x-axis is labeled "Predicted mean (LOOCV)," both with numerical values. There is an indication of a positive trend as the predicted mean increases with the training sample. Below the plot, there is a caption "Heteroscedastic nugget variance." The sidebar of the image contains menu options with various acronyms and a selected option "RES."
         :align: center
         :width: 600
         :figclass: align-center

         RES tab - Leave-one-out cross-validation error measure

Please see the :ref:`User Guide <lblSimSurrogate>` for more details on the verification measures. 

11. The LOOCV predictions can be compared for different input realizations under "Data Values" tab.

      .. figure:: figures/EE09_RES3.png
         :name: UQ inputs
         :alt: Screenshot of a computer interface displaying statistical analysis data. On the left side, there is a scatter plot with many data points suggesting a trend and a correlation coefficient of 0.76 displayed. To the right, a data table with multiple columns shows various numerical values, with column headers like "1-PFD-1-1", "1-PFD-1-2", "1-PID-1-1", among others. Above the table, there are buttons labeled "Save Table," "Save Columns Separately," "Save RVs," "Save QoIs," and "Save Surrogate Predictions." The interface has a structured design with a dark sidebar containing options like "UQ," "GI," "SIM," and others highlighted, with "RES" selected at the bottom.
         :align: center
         :width: 100%
         :figclass: align-center

         RES tab - Scatter plots

      .. note::

         The user can interact with the plot as follows.

         - Windows: left-click sets the Y axis (ordinate).  right-click sets the X axis (abscissa).
         - MAC: fn-clink, option-click, and command-click all set the Y axis (ordinate).  ctrl-click sets the X axis (abscissa).

12. The surrogate model can be saved in a json file by clicking the "Save GP Model" button at the bottom of the "Summary" tab. One main file and one auxiliary folder will be saved.

      .. figure:: figures/EE09_RES_buttons.png
         :name: UQ inputs
         :alt: "A user interface element labeled 'Saving Options' with four blue button options: 'Save GP Model,' 'Save GP Info,' 'RV Data,' and 'QoI Data.'"
         :align: center
         :width: 600
         :figclass: align-center

         RES tab - Save GP

      .. figure:: figures/EE09_RES4.jpg
         :name: UQ inputs
         :alt: Screenshot of a computer file explorer window showing a folder named "SurrogateModel" with two items inside, "templatedir_SIM" folder and "SimGpModel.json" file, with a side panel on the right indicating "Select a file to preview."
         :align: center
         :width: 900
         :figclass: align-center

         RES tab - Trained surrogate model

   - **SimGPModel.json** : This file contains information required to quickly reconstruct the surrogate model and predict the response for different input realizations. This can be later imported into EEUQ.
   - **tmplatedir_SIM** : This folder contains all the scripts and commands to run the original dynamic time history analysis. This folder can later be imported into EEUQ along with the surrogate model to alternate between original simulations and surrogate predictions or compare the surrogate predictions to the response of the original model. 

   The button "Save GP Info" will additionally allow users to save GP information, e.g. calibrated hyper-parameter values.


.. note::

   :ref:`Example 10<eeuq-0010>` demonstrates how to use the trained surrogate model for UQ analysis.
