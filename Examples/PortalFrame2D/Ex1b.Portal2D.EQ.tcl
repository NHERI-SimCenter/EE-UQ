# --------------------------------------------------------------------------------------------------
# Example 1. portal frame in 2D
# dynamic earthquake analysis of Portal Frame, with gravity.
# all units are in kip, inch, second
# elasticBeamColumn ELEMENT
#		Silvia Mazzoni & Frank McKenna, 2006
#
#    ^Y
#    |
#    3_________(3)________4       __ 
#    |                                    |          | 
#    |                                    |          |
#    |                                    |          |
#  (1)                                 (2)       LCol
#    |                                    |          |
#    |                                    |          |
#    |                                    |          |
#  =1=                               =2=      _|_  -------->X
#    |----------LBeam------------|
#

# SET UP ----------------------------------------------------------------------------
wipe;						# clear opensees model
model basic -ndm 2 -ndf 3;				# 2 dimensions, 3 dof per node
file mkdir Data; 					# create data directory

# define GEOMETRY -------------------------------------------------------------
# nodal coordinates:
node 1 0 0;					# node#, X Y
node 2 504 0
node 3 0 432
node 4 504 432 

# Single point constraints -- Boundary Conditions
fix 1 1 1 1; 			# node DX DY RZ
fix 2 1 1 1; 			# node DX DY RZ
fix 3 0 0 0
fix 4 0 0 0

# nodal masses:
pset floorMass 5.18
mass 3 $floorMass 0. 0.;					# node#, Mx My Mz, Mass=Weight/g.
mass 4 $floorMass 0. 0.

# Define ELEMENTS -------------------------------------------------------------
# define geometric transformation: performs a linear geometric transformation of beam stiffness and resisting force from the basic system to the global-coordinate system
geomTransf Linear 1;  		# associate a tag to transformation

pset E 4227
# connectivity: (make A very large, 10e6 times its actual value)
element elasticBeamColumn 1 1 3 3600000000 $E 1080000 1;	# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 2 2 4 3600000000 $E 1080000 1
element elasticBeamColumn 3 3 4 5760000000 $E 4423680 1

# define GRAVITY -------------------------------------------------------------
pattern Plain 1 Linear {
   eleLoad -ele 3 -type -beamUniform -7.94 ; # distributed superstructure-weight on beam
}
constraints Plain;     				# how it handles boundary conditions
numberer Plain;					# renumber dof's to minimize band-width (optimization), if you want to
system BandGeneral;				# how to store and solve the system of equations in the analysis
test NormDispIncr 1.0e-8 6 ; 				# determine if convergence has been achieved at the end of an iteration step
algorithm Newton;					# use Newton's solution algorithm: updates tangent stiffness at every iteration
integrator LoadControl 0.1;				# determine the next time step for an analysis, # apply gravity in 10 steps
analysis Static;					# define type of analysis static or transient
analyze 10;					# perform gravity analysis
loadConst -time 0.0;				# hold gravity constant and restart time

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


