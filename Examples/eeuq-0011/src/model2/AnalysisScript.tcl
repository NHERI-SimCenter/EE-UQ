# Perform the analysis
numberer RCM
constraints Transformation
system Umfpack
integrator Newmark 0.5 0.25
test NormUnbalance 1.0e-2 10 
algorithm Newton
analysis Transient -numSubLevels 2 -numSubSteps 10
#set lambdaN [eigen 1];
set lambdaN [eigen -fullGenLapack 1]

set lambda1 [lindex $lambdaN 0]


set T1 [expr 2*3.14159/$lambda1]
set dTana [expr $T1/20.]
if {$dt < $dTana} {set dTana $dt}
analyze [expr int($numStep*$dt/$dTana)] $dTana 
remove recorders 
