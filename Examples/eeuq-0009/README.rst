
Surrogate Model for Structural Response Prediction (Gaussian Process)
===========================================================================================================

Training Gaussian Process Surrogate Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example shows how to train a surrogate model that can replace structural dyanmic simulations once it is trained using a small set of simulations.

The ground motions are selected from the PEER NGA database

.. warning:: To reproduce the result of this example, user should first click **EVT** and **Select Records**, and then click the **RUN** button. See the below procedure for details.

Set Up GP Surrogate Modeling Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In **UQ tab** select the **Surrogate Modeling** as **UQ Method**. Select the **Train GP Surrogate Model** in the method.

      .. figure:: figures/EE09_UQ.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

   Below are the details of the selected inputs:

   - **Number of Samples** : same as the ground motions to be selected in the EVT tab, 81. This can be flexible when stochastic ground motion is selected. Typically, as the number of samples increases, surrogate modeling accuracy will increase
   - **Max Computation Time** : 60 mins (default)
   - **Target Accuracy** : 0.02. EE-UQ will give warning when this accuracy target is not met
   - **Random Seed** : Random seed generator used to reproduce the exact random samples
   - **Parallel Execution** : True. This will perform dynamic simulations in parallel

   - **Advanced Options for Gaussian Process Model** : False. By default, EE-UQ takes **Martern 5/2**, **Log-space transform of QoI**, **Heteroskedastic nugget variance** options.
   - **Existing Data set** : False. This is useful when one hopes to resume the surrogate model training after training is finished by performing more simulations
   - **Intensity Measure Calculation**: Select the intensity measures to be used as the input of surrogate model.

Define Target Structure
^^^^^^^^^^^^^^^^^^^^^^^

2. The **GI tab** is kept as default.

3. In **SIM tab**, the specifics of the target structural model is provided via **MDOF** building generator. One story building is created having stiffness as an input parameter.


      .. figure:: figures/EE09_SIM.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center


Select ground motions for the training
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

4. In **EVT tab**, **PEER NGA ground motion records** is selected. To allow the surrogate models to cover wide range of intensity measure domain, let us select **No Spectrum - Uniform IMs** for the analysis. Set the **Number of samples per bin** 1, and add two intensity measures that are used for the surrogate model training, "5-75% Significant Duration" and "Pseudo Spectral Acceleration (with natural period of 0.5 sec)". Set the coverage of former [2.5, 30] and the latter [0.1, 3.5]. For both IMs, the number of bins is set as 9. Therefore the total number of bins is 81.

      .. figure:: figures/EE09_EVT1.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center


.. warning::   

   Due to copyright issues, PEER imposes a strict limit on the number of records that can be downloaded within a unique time window. The current limit is set at approximately 200 records every two weeks, 400 every month. Please make sure this limit is not exceeded. Otherwise, the analysis will fail.

* **Temporary Records Directory** is where the downloaded data is stored. 
* **Acceleration Components** option is used to select the components to be used in the analysis. For example, if H1 is selected, single direction ground motion will be excited to the structure.
* Press **Select Records** when ready.

      .. figure:: figures/EE09_EVT2.png
         :name: UQ inputs
         :align: center
         :width: 900
         :figclass: align-center

      .. figure:: figures/EE09_EVT3.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center
         
5. The **FEM tab** is kept as default.

6. The **EDP tab** is kept as default. For the surrogate model to be compatible with the PBE and other applications, it should follow the naming of the Standard Earthquake.

Set up Training Domain
^^^^^^^^^^^^^^^^^^^^^^

6. In **RV tab** set the range of stiffness to be [50,150] as shown in the image. The selected intensity measures, "5-75% Significant duration (sec)" and "Spectral Acceleration (g)" will be additional inputs of the surrogate model.

Run the analysis and process results.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

6. Click **Run** button. The analysis may take several minutes.
