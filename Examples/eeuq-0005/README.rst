.. _eeuq-0005:

Synthetic Ground Motion Applied to a Shear Buildilng
=======================================================

Consider the problem of uncertainty quantification in a three-story shear building where the ground shaking will be 
represented by synthetic ground motion records based on two methods ([VPD18]_, [DD18]_)

.. figure:: figures/model2.png
   :align: center
   :alt: A diagram depicting a three storyone bay  structure with three horizontally aligned rigid beams. The first and second floors have a mass labeled "w". At the top floor, the mass is labeled "w/2". The columns at the based end by a symbol indicating a fixed support.
   :width: 400
   :figclass: align-center

   Three Story Shear Building Model (P10.2.9, "Dynamics of Structures", A.K.Chopra)

The exercise will use the :ref:`lblOpenSeesSIM` structural generators. For the OpenSees generator the following model script, :eeuq-0005:`ShearBuilding3.tcl <src/ShearBuilding3.tcl>` is used:

.. literalinclude:: ShearBuilding3.tcl
   :language: tcl

.. warning::

   Do not place the file in your root, downloads, or desktop folder as when the application runs it will copy the contents on the directories and subdirectories containing this file multiple times (a copy will be made for each sample specified). If you are like us, your root, Downloads or Documents folders contain an awful lot of files and when the backend workflow runs you will slowly find you will run out of disk space!

Vlacho-Papakonstantinou-Deodatis Method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We will first demonstrate the steps to perform a sampling analysis to study the effects of earthquake magnitude, site-source distance, and soil velocity 
on structural dynamic responses, using the Vlacho-Papakonstantinou-Deodatis synthetic ground motion model. For a Sampling or Forward propagation uncertainty analysis, the user would perform the following steps:

1. Upon opening the application the **UQ** tab will be highlighted. In this panel, keep the **UQ engine** as that selected, 
   i.e. Dakota, and the **UQ Method Category** as **Forward Propagation**, and the **Method** field as **LHS** 
   (Latin Hypercube). Change the **#samples** field to ``100`` as shown in the following figure.

.. figure:: figures/uq.png
   :align: center
   :alt: Screenshot of a software application titled "EE-UQ: Response of Building to Earthquake" with a user interface comprising various tabs on the left, such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES. The UQ tab is currently selected and displays settings for a 'UQ Engine' named 'Dakota', including 'Dakota Method Category' set to 'Forward Propagation' and 'Method' set to 'LHS'. There are fields for 'Number of Samples' set to 100, 'Seed' set to 530, and an unchecked option to 'Keep Samples'. There are buttons at the bottom for 'RUN', 'RUN at DesignSafe', 'GET from DesignSafe', and 'Exit'. Additionally, there is a 'Login' button at the top right corner.
   :width: 100%
   :figclass: align-center

2. The **GI** panel will not be used for this run; For the time being leave the default values as is, and they will be automatically updated based on the information entered in the remaining tabs.

3. Next select the **SIM** panel from the input panel. This will default in the MDOF model generator. 
   From the **Model Generator** pull-down menu select **OpenSees** and select the example 
   :eeuq-0005:`3-story frame model </src/ShearBuilding3.tcl>`. For recording story responses, we also specify the 
   node numbers on a column line (e.g., 1, 2, 3, 4 in the OpenSees model).

.. figure:: figures/sim.png
   :align: center
   :alt: Screenshot of a software interface titled "EE-UQ: Response of Building to Earthquake". The interface includes a section labeled "Building Model Generator" with options for software selection and input fields for an input script, response nodes, spatial dimension, and degrees of freedom at nodes. A navigation panel on the left lists categories like UQ, SIM, EVT, FEM, EDP, RV, and RES. At the bottom are buttons for running simulations, including "RUN", "RUN at DesignSafe", and "GET from DesignSafe". There is also a "Login" button on the top right corner of the window.
   :width: 100%
   :figclass: align-center

4. Next select the **EVT** panel. From the **Load Generator** pull-down menu select the **Stochastic Ground Motion** 
   option. This will present a new page of stochastic loading models. We first use the "Vlachos et al. (2018)" and 
   for the three earthquake parameters: magnitude, distance, and shear-wave velocity, we define variables that 
   will be randomized later in the **RV** panel

.. figure:: figures/evt.png
   :align: center
   :alt: Screenshot of a software interface titled "EE-UQ: Response of Building to Earthquake" with multiple tabs and input fields related to stochastic ground motion. The displayed tab, "Load Generator," includes a dropdown menu for selecting a stochastic loading model and input fields for moment magnitude, closest-to-site rupture distance, and average shear-wave velocity, with a provided seed value field set to 500. There are buttons for running the analysis and additional options for DesignSafe integration at the bottom.
   :width: 100%
   :figclass: align-center

5. Next choose the **FEM** panel. Here we will change the entries to use Rayleigh damping, with the Rayleigh factor chosen using the 
   first and second modes. 

.. figure:: figures/fem.png
   :align: center
   :alt: A screenshot of a software interface titled "EE-UQ: Response of Building to Earthquake". The interface has a menu bar at the top with options like File, Help, and Windows, and a login button at the top-right corner. On the left side, there's a vertical navigation menu with items such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES highlighted. The main part of the window shows options for a Finite Element (FE) Application with various parameters for analysis, integration, algorithm, convergence test, solver, damping model, and selected tangent stiffness. There are input fields, dropdown menus, and a 'Choose' button for an analysis script. At the bottom, there are buttons to "RUN", "RUN at DesignSafe", "GET from DesignSafe", and "Exit". The interface design is clean with mostly grayscale colors and blue highlights.
   :width: 100%
   :figclass: align-center

6. We will skip the **EDP** panel leaving it in it's default condition, that being to use the **Standard Earthquake** EDP generator.

7. For the **RV** panel, we will enter the distributions and values for our random variables. For demonstration purposes, 
   we assume all three variables follow normal distributions: the magnitude has a mean of 7 and a standard deviation of 1; 
   the distance has a mean of 20 km and standard deviation of 5 km; and the soil velocity has a mean of 259 m/s and 
   standard deviation of 50 m/s.

.. figure:: figures/rv.png
   :align: center
   :alt: A screenshot of an engineering software interface titled "EE-UQ: Response of Building to Earthquake." The screen displays a section with "Input Random Variables" where three variables (M, R, V) are listed with their corresponding distribution type (Normal), mean values (7, 20, 259), and standard deviations (1, 5, 50), with buttons to "Show PDF" for each variable. On the left side, there is a vertical menu with options such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES highlighted. At the bottom, there are buttons for "RUN," "RUN at DesignSafe," and "GET from DesignSafe," along with an "Exit" button on the bottom right. The user is not logged in as indicated by the Login button in the top right corner.
   :width: 100%
   :figclass: align-center

8. Next click on the **Run** button. This will cause the backend application to launch Dakota. When the analysis is complete the **RES** tab will be selected and the results will be displayed. The results show the values for the mean and standard deviation. 
   
.. figure:: figures/res1.png
   :align: center
   :alt: Screenshot of a software interface titled "EE-UQ: Response of Building to Earthquake," displaying tabs for 'UQ,' 'GI,' 'SIM,' 'EVT,' 'FEM,' 'EDP,' 'RV,' and 'RES'. The visible tab shows a table with statistical data including 'Name', 'Mean', 'StdDev', 'Skewness', and 'Kurtosis' for various building response parameters. The interface includes navigation buttons 'RUN', 'RUN at DesignSafe', and 'Exit' at the bottom.
   :width: 100%
   :figclass: align-center

If the user selects the **Data** tab in the **RES** panel, they will be presented with both a graphical plot 
and a tabular listing of the data. By left- and right-clicking with the mouse in the individual columns 
the axis changes (the left mouse click controls the vertical axis, right mouse clicks the horizontal axis). For example, 
we can now see the influences from the magnitude (:math:`M`), distance (:math:`R`), and soil velocity (:math:`V`) on the 1st story peak story drift 
ratio (:math:`1-PID-1-1`).

.. figure:: figures/inf_m1.png
   :align: center
   :alt: The image is a scatter plot with a horizontal axis labeled "1-PID-1-1" and a vertical axis labeled "M." The horizontal axis ranges from approximately 0.0003 to 0.0309, and the vertical axis ranges from approximately 4.1 to 10.3. The plot features a distribution of individual points, primarily concentrated between the 0.003 and 0.0156 marks on the horizontal axis, and between the 5.6 and 7.2 marks on the vertical axis, suggesting a potential correlation or trend within that particular range. There are a few outlier points scattered horizontally towards the higher end of the horizontal axis. The background is white with gray grid lines, and the points are shaded in a light blue color.
   :width: 100%
   :figclass: align-center

.. figure:: figures/inf_r1.png
   :align: center
   :alt: A scatter plot with a cluster of data points along the horizontal axis labeled '1-PID-1-1' ranging approximately from 0.0003 to 0.0309 and along the vertical axis labeled 'R' ranging approximately from 4.1 to 34.6. The data points are concentrated mostly within the middle of the graph, suggesting some form of correlation or data distribution pattern. The background is white with labeled axes, and the data points are blue.
   :width: 100%
   :figclass: align-center

.. figure:: figures/inf_v1.png
   :align: center
   :alt: A scatter plot with numerous light blue dots representing data points. The horizontal axis is labeled "1-PID-1-1" and ranges from approximately 0.0003 to 0.0309. The vertical axis, labeled with a right-pointing arrow, indicates a range from about 85.6 to 447.2, though the specific units are not visible. The distribution of dots does not show a clear pattern or correlation, with a somewhat even spread across the graph canvas. No trend line is visible.
   :width: 100%
   :figclass: align-center


Dabaghi-Der Kiureghian Method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Dabaghi-Kiureghian model looks into the near-fault ground motion pulse feature and can generate pairs of 
synthetic pulse-like and non-pulse-like components in appropriate proportions. In this example, we would like to 
(1) understand the effects from the randomness in the site-source distance (as similar 
but comparable to the first example above) and (2) compare the differences in structural responses due to the 
distinct characteristics of pulse-like and non-pulse-like records.

1. All the configurations are maintained the same as the first example except in the **EVT** panel, we select the 
   "Dabaghi & Der Kiureghian (2018)" method. We first fix the fault type to "Stike-Slip" and enable both the 
   pulse-like and non-pulse-like records.

2. For the Depth to the Top of the Rupture Plane, we specify 0.0 km as a constant; and for the other earthquake parameters (
   moment magnitude,  closest distance to the site, parallel distance, and angel), we define their values as the figure 
   shown below. The random variable average shear-wave velocity (V) will be specified in the **RV** panel.

.. figure:: figures/evt2.png
   :align: center
   :alt: Screenshot of a software interface titled "EE-UQ: Response of Building to Earthquake" displaying various input fields and settings for simulating earthquake responses on structures. On the left side of the interface, there's a column with sections like "Load Generator", "EVT", "RV", and "RES" which include drop-down menus and numerical input fields for selecting earthquake characteristics such as the "Stochastic Loading Model," "Moment Magnitude," and "Average shear-wave velocity." On the right side, there are schematic diagrams illustrating "Strike Slip" and "Dip Slip" fault movements with a "Vertical Section" and "Plan View," showing the relationship between the fault lines and a site on the surface. Below the input sections, there's a "RUN" button and additional options to "RUN at DesignSafe" or "Get from DesignSafe." At the top right corner of the window is a "Login" button.
   :width: 100%
   :figclass: align-center

3. In the **RV** panel, we specify the normal distribution for the R variable.

.. figure:: figures/rv2.png
   :align: center
   :alt: Screenshot of a software interface for "EE-UQ: Response of Building to Earthquake" featuring a menu to input random variables. The interface is currently displaying one variable named "V" with a normal distribution, a mean value of 259, and a standard deviation of 50. There are options to add and remove variables as well as a "Show PDF" button. On the left sidebar, options such as UQ, GI, SIM, EVT, FEM, EDP, RV, and RES are selectable, with 'RV' currently highlighted. At the bottom, there are buttons for "RUN," "RUN at DesignSafe," "GET from DesignSafe," and "Exit." The "Login" option is available on the top right corner.
   :width: 100%
   :figclass: align-center

4. In the **RES** panel, similar to the first example, the maximum story drift ratio and peak acceleration demands are
   recorded (e.g., 1st story 1-PID-1-1 and 3rd floor 1-PFA-3-1). And we could examine the influence of the site soil 
   velocity on the 1st story peak story drift ratio.

.. figure:: figures/all_sdr1.png
   :align: center
   :alt: A line graph showing a cumulative probability distribution. The x-axis is labeled "1-PID-1-1" with values ranging from approximately -0.0008 to 0.002, and the y-axis is labeled "Cumulative Probability" with values ranging from 0.00 to 1.00. The line progresses steeply upwards in the negative range of the x-axis and flattens as it approaches the positive range, indicating a high probability as one moves from left to right on the graph.
   :width: 100%
   :figclass: align-center

.. figure:: figures/all_pfa3.png
   :align: center
   :alt: The image is a line graph displaying a cumulative probability distribution. The x-axis is labeled with numerical values ranging from approximately -82.4 to 1958.7, and the y-axis ranges from 0.00 to 1.00 in increments of 0.25, symbolizing probability. The graph features a continuously ascending line starting near the origin and flattening out as it approaches a cumulative probability of 1. It suggests a dataset where values accumulate towards a maximum probability of 1, characteristic of a cumulative distribution function. The specific details on the x-axis (e.g., '1-PFA-3-1') cannot be interpreted without additional context.
   :width: 100%
   :figclass: align-center

.. figure:: figures/inf_v2.png
   :align: center
   :alt: A scatter plot with numerous points plotted on a Cartesian coordinate system. The horizontal axis is labeled '1-PID-1-1' with values ranging from approximately 0.0008 to 0.0202. The vertical axis is on the left side and appears to represent some quantity or measurement scale, possibly in the hundreds, with values varying from about 106.6 to 434.9. The distribution of points does not suggest a clear trend or correlation, as they are spread out across the chart without forming a distinct pattern.
   :width: 100%
   :figclass: align-center

5. The second part of this example is to compare the pulse-like and non-pulse-like records. So, we navigate back 
   to the **EVT** panel and first select the "Only pulse-like" and then the "Non-pulse-like only" to run the analysis 
   again. As seen in the figure below, the non-pulse-like group, on average, results in 10% to 15% higher structural responses in this 
   example.

.. figure:: figures/sdr_pfa_comp.png
   :align: center
   :alt: Two cumulative distribution function (CDF) plots side by side. The left plot shows the CDF of 'Roof PFA (g)' with lines representing 'Pulse-like' in blue and 'Non-pulse-like' in orange. The right plot shows the CDF of '1st story PID (in/in)' also with 'Pulse-like' in blue and 'Non-pulse-like' in orange. The plots are designed to compare the distribution of peak floor acceleration and peak inter-story drift for pulse-like and non-pulse-like scenarios, indicated by the reference lines and color key in the legend below each graph.
   :width: 100%
   :figclass: align-center

.. [VPD18]
   Vlachos, C., Papakonstantinou, K. G., & Deodatis, G. (2018). Predictive model for site-specific simulation of ground motions based on earthquake scenarios. Earthquake Engineering & Structural Dynamics, 47(1), 195-218.

.. [DD18]
   Dabaghi, M., & Der Kiureghian, A. (2018). Simulation of orthogonal horizontal components of near-fault ground motion for specified earthquake source and site characteristics. Earthquake Engineering & Structural Dynamics, 47(6), 1369-1393.
