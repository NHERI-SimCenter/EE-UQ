source ./Frame1Model.tcl

# DYNAMIC ground-motion analysis -------------------------------------------------------------
# create load pattern
timeSeries Path 1 -dt 0.01 -filePath BM68elc.acc -factor 386.220472;	# define acceleration vector from file (dt=0.01 is associated with the input file gm)
pattern UniformExcitation 2 1 -accel 1;		# define where and how (pattern tag, dof) acceleration is applied
set xDamp 0.02;
set MpropSwitch 1.0;
set KcurrSwitch 0.0;
set KinitSwitch 0.0;
set KcommSwitch 1.0;
set nEigenI 1;
set nEigenJ 2;
set lambdaN [eigen -fullGenLapack 2];
set lambdaI [lindex $lambdaN [expr $nEigenI-1]];
set lambdaJ [lindex $lambdaN [expr $nEigenJ-1]];
set omegaI [expr pow($lambdaI,0.5)];
set omegaJ [expr pow($lambdaJ,0.5)];
set alphaM [expr $MpropSwitch*$xDamp*(2*$omegaI*$omegaJ)/($omegaI+$omegaJ)];
set betaKcurr [expr $KcurrSwitch*2.*$xDamp/($omegaI+$omegaJ)];
set betaKinit [expr $KinitSwitch*2.*$xDamp/($omegaI+$omegaJ)];
set betaKcomm [expr $KcommSwitch*2.*$xDamp/($omegaI+$omegaJ)];
rayleigh $alphaM $betaKcurr $betaKinit $betaKcomm;

# Define RECORDERS -------------------------------------------------------------
recorder EnvelopeNode -file PFA.out -timeSeries 1 -node 3 -dof 1 accel;		# Peak Floor Absolute Acceleration
recorder EnvelopeDrift -file PID.out -iNode 1 -jNode 3 -dof 1 -perpDirn 2

# create the analysis
wipeAnalysis;					# clear previously-define analysis parameters
constraints Plain;     				# how it handles boundary conditions
numberer Plain;					# renumber dof's to minimize band-width (optimization), if you want to
system BandGeneral;				# how to store and solve the system of equations in the analysis
test NormDispIncr 1.0e-8 10;				# determine if convergence has been achieved at the end of an iteration step
algorithm Newton;					# use Newton's solution algorithm: updates tangent stiffness at every iteration
integrator Newmark 0.5 0.25 ;			# determine the next time step for an analysis
analysis Transient;					# define type of analysis: time-dependent
analyze 4000 0.01;					# apply 1000 0.02-sec time steps in analysis

puts "Done!"


