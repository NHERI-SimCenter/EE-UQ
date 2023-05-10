.. _eeuq-0010:

Structural Response Prediction Using a Surrogate Model (Gaussian Process)
===========================================================================================================

.. warning:: To reproduce the result of this example, the user should first click **EVT** and **Select Records**, and then click the **RUN** button. See the below procedure for details.

This example shows how to replace structural dynamic simulations using a pre-trained Gaussian process (GP) surrogate model for running forward uncertainty propagation (Monte Carlo Simulation). The ground motions are selected from the PEER NGA database matching the Design Spectrum of ASCE 7-16 standard.

      .. figure:: figures/EE10_main2.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         Prediction of response statistics using a surrogate model

.. important:: This examples uses the surrogate model trained in :ref:`example 09<eeuq-0009>`

Set Up Forward Propagation Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In **UQ tab** select the **Forward modeling** as **UQ Method**. Select **Dakota** engine.

      .. figure:: figures/EE10_UQ.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         UQ tab - Scatter plots

   We will select 30 samples. If recorded ground motions will be used as input excitation, as in this example, the number of ground motions that will be selected in the EVT tab should match the number of samples specified in this tab. This restriction does not apply when a stochastic ground motion generator is used instead of the recorded ground motions.
   
Define Target Structure
^^^^^^^^^^^^^^^^^^^^^^^

2. The **GI tab** is kept as default. (GI tab is not used when surrogate model is used in SIM tab)

3. In **SIM tab**, the surrogate model (.json) trained in :ref:`Example 09<eeuq-0009>` is imported as shown in the figure.

      .. figure:: figures/EE10_SIM.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         SIM tab

   .. note::   

      :ref:`Example 09<eeuq-0009>` describes how to train GP surrogate model and save it as `.json` format. 

   When the option "Random sample under prediction uncertainty" is selected, the predictions from GP are random realizations that account for both model uncertainty and a portion of uncertainty in the ground motion time histories (i.e. the remaining uncertainty after given intensity measures (IMs)). Alternatively, when the user is interested in only the mean of the response, disregarding all the uncertainties, the user can select "Median (representative) prediction".

Select Ground Motions
^^^^^^^^^^^^^^^^^^^^^^

4. In **EVT tab**, **PEER NGA ground motion records** is selected. Let us consider the site of interest located at (37.8715, -122.273), Hard Rock (A) site class. From ASCE 7-16, the design spectrum for risk category I (low hazard) can be obtained. Let us select 30 ground motion time histories that match this spectrum by clicking **Select records**button. The target response spectrum curve and the selected ground motions will be displayed in the right-hand side panel as shown below.

      .. figure:: figures/EE10_EVT1.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         EVT tab - target response spectrum

      .. figure:: figures/EE10_EVT3.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center

         EVT tab - selected ground motion records on the response spectrum curve
         
The list of the selected ground motions is shown in the table.

      .. figure:: figures/EE10_EVT2.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center

         EVT tab - temporary records directory and scaling options

The actual time histories are saved in the "Temporary Records Directory".

.. warning::   

   Due to copyright issues, PEER imposes a strict limit on the number of records that can be downloaded within a unique time window. The current limit is set at approximately 200 records every two weeks, 400 every month. Please make sure this limit is not exceeded. Otherwise, the analysis will fail.

* **Temporary Records Directory** is where the downloaded ground motion records are stored. It is recommended to specify a directory here instead of using the default temporary directory, in order to reuse the time history data in future analysis. 
* **Acceleration Components** option is used to select the directional components to be used in the analysis. For example, if H1 is selected, single-direction ground motion will be excited to the structure.

5. In **FEM tab** select **None (only for surrogate)**.

      .. figure:: figures/EE10_FEM.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         FEM tab

6. The **EDP tab** is automatically populated with the EDP names used when training the surrogate. Keep the option  **None (only for surrogate)**

      .. figure:: figures/EE10_EDP.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         EDP tab

Distribution of Random Variables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

7. The **RV tab** is pre-populated with the random variables that were used to train the surrogate. Change the distribution of the statistical parameters as desired. In this example, the stiffness is assumed to be distributed around 120 with a standard deviation of 5.

      .. figure:: figures/EE10_RV.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         RV tab

.. warning:: Note that the surrogate modeling is essentially based on "interpolation". Therefore, the distribution of stiffness should not significantly exceed the training bound. If a sampled stiffness value lies outside of the training range, [50, 150] in this example, the prediction from the surrogate model for that sample is likely not reliable.


Run the Analysis and Process Results
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

8. Click **Run** button. The analysis may take several minutes. The **RES tab** will be highlighted when the analysis is completed

9. The obtained statistics of responses are shown in the "Summary tab"

      .. figure:: figures/EE10_RES1.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         RES tab - summary of response statistics

10. In the "Data Values" tab, one can plot the histogram and cumulative density function (CDF) of the samples, as well as scatter plots between the input and output of surrogate predictions

   .. figure:: figures/EE10_RES2.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         RES tab - cumulative density function

   .. figure:: figures/EE10_RES3.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         RES tab - scatter plots

   .. note::

         The user can interact with the plot as following.

         - Windows: left-click sets the Y axis (ordinate).  right-click sets the X axis (abscissa).
         - MAC: fn-clink, option-click, and command-click all set the Y axis (ordinate).  ctrl-click sets the X axis (abscissa).

   In the scatter plot, the gray square markers represent the mean prediction from the surrogate, gray bounds denote 90% prediction interval, orange bounds denote 90% confidence interval of the mean prediction, and blue dots represent the sample obtained from the surrogate prediction. 

   .. note::
      * The term "90% prediction interval" is the interval in which the exact "response", i.e. dynamic simulation output, will fall with 90% probability.
      * The term "90% confidence interval" is the estimated range of the "mean response". Therefore, the confidence interval is always tighter than the prediction interval.

11. **[Verification]** Only for verification purposes, an additional forward propagation is performed using the exact simulation model instead of the surrogate model, using the exact same ground motion/structural parameters. For this, UQ, GI, EVT, RV tabs are kept unchanged, and SIM, FEM, EDP tabs are modified to replace the surrogate with the original model, i.e. for SIM, FEM, EDP tabs, the exact same configuration used in :ref:`example 09<eeuq-0009>` was used.  Below is a comparison of the obtained median (and log-mean) and log-standard deviation of the EDPs from 30 samples:


   .. figure:: figures/EE10_RES4_1.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

         Comparison of median

   .. figure:: figures/EE10_RES4_2.png
         :name: UQ inputs
         :align: center
         :width: 400
         :figclass: align-center

         Comparison of log-standard deviation

The estimated medians of EDPs from the surrogate and the original model show in general good agreement. The standard deviation of the surrogate model is larger partly because of the added uncertainty in surrogate model approximation. The difference in the statistics may also attribute to the small sample size of 30.
