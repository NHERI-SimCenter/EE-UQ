
Surrogate Model for Structural Response Prediction (Gaussian Process)
===========================================================================================================

Training Gaussian Process Surrogate Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example shows how to train a surrogate model that can replace structural dyanmic simulations once it is trained using a small set of simulations.

The ground motions are selected from the PEER NGA database

.. warning:: To reproduce the result of this example, user should first click **EVT** and **Select Records**, and then click the **RUN** button. See the below procedure for details.

Configure Surrogate Target Spectrum
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In **UQ tab** select the **SimCenterUQ** as **UQ Engine**. Select the **Train GP Surrogate Model** in the method.

      .. figure:: figures/EE09_UQ.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center

   Below are the details of the selected inputs:

   - **Number of Samples** : same as the ground motions to be selected in the EVT tab, 64. This is has freedom when stochastic ground motion is selected. Typically, as the number of samples increases, surrogate modeling accuracy will increase
   - **Max Computation Time** : 60 mins (default)
   - **Target Accuracy** : 0.02. EE-UQ will give warning when this accuracy target is not met
   - **Random Seed** : Random seed generator used to reproduce the exact random samples
   - **Parallel Execution** : True. This will perform dynamic simulations in parallel

   - **Advanced Options for Gaussian Process Model** : False. By default, EE-UQ takes **Martern 5/2**, **Log-space transform of QoI**, **Heteroskedastic nugget variance** options.
   - **Existing Data set** : False. This is useful when one hopes to resume the surrogate model training after training is finished by performing more simulations
   - **Intensity Measure Calculation**: Select the intensity measures to be used as the input of surrogate model.

2. The **GI tab** is kept as default.

3. In **SIM tab**, the specifics of the target structural model is provided via **MDOF** building generator. One story building is created having stiffness as an input parameter.


      .. figure:: figures/EE09_SIM.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center


4. In **EVT tab**, **PEER NGA ground motion records** is selected. Theoretically the more diverse the intensity measures are, the more likely to reasonably capture the aleatory uncertainty in the earthquake source. 


      .. figure:: figures/EE09_EVT1.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center


      .. figure:: figures/EE09_EVT2.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center


      .. figure:: figures/EE09_EVT3.png
         :name: UQ inputs
         :align: center
         :width: 600
         :figclass: align-center
         
5. The **FEM tab** is kept as default.

6. The **EDP tab** is kept as default. For the surrogate model to be compatible with the PBE and other applications, it should follow the naming of the Standard Earthquake.

