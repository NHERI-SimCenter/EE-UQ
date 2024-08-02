# LA Post-Northridge 3 story North-South no cover plates, no column splice
# from FEMA-355C/Sept. 2000  (https://www.nehrp.gov/pdf/fema355c.pdf)
#
# units kips,inches
# load procedures in other files

source Steel2d.tcl

# set some variables
set in 1.0;
set g 386.4;				# acceleration due to gravity

pset Es1 29000.0;  # modulus of elasticity for steel
pset Es2 29000.0;  # modulus of elasticity for steel
pset Es3 29000.0;  # modulus of elasticity for steel
pset Esb 29000.0

pset Fy1 50.0; 	 # yield stress of steel
pset Fy2 50.0; 	 # yield stress of steel
pset Fy3 50.0; 	 # yield stress of steel
pset Fyb 50.0

pset b1 0.01;	 # strain hardening ratio
pset b2 0.01;	 # strain hardening ratio
pset b3 0.01;	 # strain hardening ratio
pset bb 0.01

# set up my lists
set floorOffsets {156. 156. 156.}
set colOffsets   {360. 360. 360.} 

set roofMass  [expr 70.9/2.]; # 2 frames per dirn
set floorMass [expr 65.53/2.]
set roofBeamLoad  0.12;  #96*30*15  lbs per external beam /360in /1000 kips  
set floorBeamLoad 0.10375; # 83*30*15 lbs per external beam /360 in /1000

set numFloor [expr [llength $floorOffsets]+1]
set numCline [expr [llength $colOffsets]+1]

set  massesCMD     "set masses {0. [expr $floorMass/$numCline] [expr $floorMass/$numCline] [expr $roofMass/$numCline]}"
eval $massesCMD

set colSizes       {W14X311 W14X311 W14X311};
set colESizes      {W14X257 W14X257 W14X257};
set beamSizes      {W30X116 W30X116 W24X62};

# build colLocations and floorLocations
set floorLocations 0; set floorLoc 0; 
set colLocations 0; set colLoc 0; 

for {set i 1} {$i < $numFloor} {incr i 1} {
    set floorLoc [expr $floorLoc + [lindex $floorOffsets [expr $i-1]]]
    lappend floorLocations $floorLoc;
}
for {set i 1} {$i < $numCline} {incr i 1} {
    set colLoc [expr $colLoc + [lindex $colOffsets [expr $i-1]]]
    lappend colLocations $colLoc;
}

# check of list dimensions for errors
if {[llength $masses] != $numFloor} {puts "ERROR: massX"; quit}
if {[llength $colSizes] != [expr $numFloor-1]} {puts "ERROR: colSizes"; quit}
if {[llength $beamSizes] != [expr $numFloor-1]} {puts "ERROR: beamSizes"; quit}
if {$numFloor >= 10} {puts "ERROR: too many floors"; quit}
if {$numCline >= 10} {puts "ERROR: too many colun lines"; quit}


model BasicBuilder -ndm 2 -ndf 3;  # Define the model builder, ndm = #dimension, ndf = #dofs

# create materials for each column story and the beams
uniaxialMaterial Steel02 1 $Fy1 $Es1 $b1 20 0.925 0.15
uniaxialMaterial Steel02 2 $Fy2 $Es2 $b2 20 0.925 0.15
uniaxialMaterial Steel02 3 $Fy3 $Es3 $b3 20 0.925 0.15
uniaxialMaterial Steel02 4 $Fyb $Esb $bb 20 0.925 0.15

# Build the Nodes
for {set floor 1} {$floor <= $numFloor} {incr floor 1} {
    set floorLoc [lindex $floorLocations [expr $floor-1]]
    
    set mass [lindex $masses [expr $floor-1]]
    
    for {set colLine 1} {$colLine <= $numCline} {incr colLine 1} {
	set colLoc [lindex $colLocations [expr $colLine-1]]
	node $colLine$floor $colLoc $floorLoc -mass $mass $mass 0.
	if {$floor == 1} {
	    fix $colLine$floor 1 1 1
	} else {
	    fix $colLine$floor 0 0 0
	}
    }
}

# build the columns
geomTransf PDelta 1
for {set colLine 1} {$colLine <= $numCline} {incr colLine 1} {
    for {set floor1 1} {$floor1 < $numFloor} {incr floor1 1} {
	set floor2 [expr $floor1+1]
	if {$colLine == 1 || $colLine == $numCline} {
	    set theSection [lindex $colESizes [expr $floor1 -1]]
	} else {
	    set theSection [lindex $colSizes [expr $floor1 -1]]
	}
	ForceBeamWSection2d $colLine$floor1$colLine$floor2 $colLine$floor1 $colLine$floor2 $theSection $floor1 1 -nip 5
	#ElasticBeamWSection2d $colLine$floor1$colLine$floor2 $colLine$floor1 $colLine$floor2 $theSection 1 1 -nip 5
	#ElasticBeamWSection2d $colLine$floor1$colLine$floor2 $colLine$floor1 $colLine$floor2 $theSection $Es 1 
    }
}

# build the beams
geomTransf Linear 2
for {set colLine1  1} {$colLine1 < $numCline} {incr colLine1 1} {
    set colLine2 [expr $colLine1 + 1]
    for {set floor 2} {$floor <= $numFloor} {incr floor 1} {
	set theSection [lindex $beamSizes [expr $floor -2]]
	ForceBeamWSection2d $colLine1$floor$colLine2$floor $colLine1$floor $colLine2$floor $theSection 4 2
	#ElasticBeamWSection2d $colLine1$floor$colLine2$floor $colLine1$floor $colLine2$floor $theSection $Es 2
	#element elasticBeamColumn $colLine1$floor$colLine2$floor $colLine1$floor $colLine2$floor 100 $Es 1e14 1
    }
}

# add uniform loads to beams
set floorLoad -0.11238
set roofLoad -0.1026
timeSeries Linear 1
pattern Plain 1 1 {
    for {set colLine1  1} {$colLine1 < $numCline} {incr colLine1 1} {
	set colLine2 [expr $colLine1 + 1]
	for {set floor 2} {$floor <= $numFloor} {incr floor 1} {
	    if {$floor == 4} {
		eleLoad -ele $colLine1$floor$colLine2$floor -type beamUniform $roofBeamLoad
	    } else {
		eleLoad -ele $colLine1$floor$colLine2$floor -type beamUniform $floorBeamLoad
	    }
	}
    }
}

# Gravity-analysis: load-controlled static analysis
set Tol 1.0e-6;    
constraints Plain;	
numberer RCM;		
system BandGeneral;	
test NormDispIncr $Tol 10;
algorithm Newton;		
integrator LoadControl 0.1;
analysis Static;		
analyze 10

# maintain constant gravity loads and reset time to zero
loadConst -time 0.0
wipeAnalysis


