# #########################################################

wipe 

# ------------------------------------------ 
# 1. Build nodes and elements                
# ------------------------------------------ 
 
model BasicBuilder -ndm 2 -ndf 3  

set nodesInfo [open nodesInfo.dat w]
node 1 0.0 0
puts $nodesInfo "1 0.0 0"
node 2 0.25 0
puts $nodesInfo "2 0.25 0"
# Layer 1: thickness = 3.00000000000000e+00, 15 elements.
node 3 0.0 2.00000000000000e-01
puts $nodesInfo "3 0.0 2.00000000000000e-01"
node 4 2.50000000000000e-01 2.00000000000000e-01
puts $nodesInfo "4 2.50000000000000e-01 2.00000000000000e-01"
source material.tcl
element SSPquadUP 1 1 2 4 3 1 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 5 0.0 4.00000000000000e-01
puts $nodesInfo "5 0.0 4.00000000000000e-01"
node 6 2.50000000000000e-01 4.00000000000000e-01
puts $nodesInfo "6 2.50000000000000e-01 4.00000000000000e-01"
element SSPquadUP 2 3 4 6 5 2 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 7 0.0 6.00000000000000e-01
puts $nodesInfo "7 0.0 6.00000000000000e-01"
node 8 2.50000000000000e-01 6.00000000000000e-01
puts $nodesInfo "8 2.50000000000000e-01 6.00000000000000e-01"
element SSPquadUP 3 5 6 8 7 3 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 9 0.0 8.00000000000000e-01
puts $nodesInfo "9 0.0 8.00000000000000e-01"
node 10 2.50000000000000e-01 8.00000000000000e-01
puts $nodesInfo "10 2.50000000000000e-01 8.00000000000000e-01"
element SSPquadUP 4 7 8 10 9 4 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 11 0.0 1.00000000000000e+00
puts $nodesInfo "11 0.0 1.00000000000000e+00"
node 12 2.50000000000000e-01 1.00000000000000e+00
puts $nodesInfo "12 2.50000000000000e-01 1.00000000000000e+00"
element SSPquadUP 5 9 10 12 11 5 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 13 0.0 1.20000000000000e+00
puts $nodesInfo "13 0.0 1.20000000000000e+00"
node 14 2.50000000000000e-01 1.20000000000000e+00
puts $nodesInfo "14 2.50000000000000e-01 1.20000000000000e+00"
element SSPquadUP 6 11 12 14 13 6 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 15 0.0 1.40000000000000e+00
puts $nodesInfo "15 0.0 1.40000000000000e+00"
node 16 2.50000000000000e-01 1.40000000000000e+00
puts $nodesInfo "16 2.50000000000000e-01 1.40000000000000e+00"
element SSPquadUP 7 13 14 16 15 7 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 17 0.0 1.60000000000000e+00
puts $nodesInfo "17 0.0 1.60000000000000e+00"
node 18 2.50000000000000e-01 1.60000000000000e+00
puts $nodesInfo "18 2.50000000000000e-01 1.60000000000000e+00"
element SSPquadUP 8 15 16 18 17 8 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 19 0.0 1.80000000000000e+00
puts $nodesInfo "19 0.0 1.80000000000000e+00"
node 20 2.50000000000000e-01 1.80000000000000e+00
puts $nodesInfo "20 2.50000000000000e-01 1.80000000000000e+00"
element SSPquadUP 9 17 18 20 19 9 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 21 0.0 2.00000000000000e+00
puts $nodesInfo "21 0.0 2.00000000000000e+00"
node 22 2.50000000000000e-01 2.00000000000000e+00
puts $nodesInfo "22 2.50000000000000e-01 2.00000000000000e+00"
element SSPquadUP 10 19 20 22 21 10 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 23 0.0 2.20000000000000e+00
puts $nodesInfo "23 0.0 2.20000000000000e+00"
node 24 2.50000000000000e-01 2.20000000000000e+00
puts $nodesInfo "24 2.50000000000000e-01 2.20000000000000e+00"
element SSPquadUP 11 21 22 24 23 11 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 25 0.0 2.40000000000000e+00
puts $nodesInfo "25 0.0 2.40000000000000e+00"
node 26 2.50000000000000e-01 2.40000000000000e+00
puts $nodesInfo "26 2.50000000000000e-01 2.40000000000000e+00"
element SSPquadUP 12 23 24 26 25 12 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 27 0.0 2.60000000000000e+00
puts $nodesInfo "27 0.0 2.60000000000000e+00"
node 28 2.50000000000000e-01 2.60000000000000e+00
puts $nodesInfo "28 2.50000000000000e-01 2.60000000000000e+00"
element SSPquadUP 13 25 26 28 27 13 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 29 0.0 2.80000000000000e+00
puts $nodesInfo "29 0.0 2.80000000000000e+00"
node 30 2.50000000000000e-01 2.80000000000000e+00
puts $nodesInfo "30 2.50000000000000e-01 2.80000000000000e+00"
element SSPquadUP 14 27 28 30 29 14 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
node 31 0.0 3.00000000000000e+00
puts $nodesInfo "31 0.0 3.00000000000000e+00"
node 32 2.50000000000000e-01 3.00000000000000e+00
puts $nodesInfo "32 2.50000000000000e-01 3.00000000000000e+00"
element SSPquadUP 15 29 30 32 31 15 1.0 2.20000000000000e+06 1.0 1.0 1.0 5.00000000000000e-01 1.00000000000000e-08 0.000000 -9.810000
close $nodesInfo


# ------------------------------------------ 
# 2. Apply boundary conditions.              
# ------------------------------------------ 
 
# 2.1 Apply fixities at base              

fix 1 1 1 0
fix 2 1 1 0

# 2.2 Apply periodic boundary conditions    

equalDOF 3 4 1 2
equalDOF 5 6 1 2
equalDOF 7 8 1 2
equalDOF 9 10 1 2
equalDOF 11 12 1 2
equalDOF 13 14 1 2
equalDOF 15 16 1 2
equalDOF 17 18 1 2
equalDOF 19 20 1 2
equalDOF 21 22 1 2
equalDOF 23 24 1 2
equalDOF 25 26 1 2
equalDOF 27 28 1 2
equalDOF 29 30 1 2
equalDOF 31 32 1 2


# 2.3 Apply pore pressure boundaries for nodes above water table. 

fix 31 0 0 1
fix 32 0 0 1


# ------------------------------------------ 
# 3. Gravity analysis.                       
# ------------------------------------------ 
 
updateMaterialStage -material 1 -stage 0
updateMaterialStage -material 2 -stage 0
updateMaterialStage -material 3 -stage 0
updateMaterialStage -material 4 -stage 0
updateMaterialStage -material 5 -stage 0
updateMaterialStage -material 6 -stage 0
updateMaterialStage -material 7 -stage 0
updateMaterialStage -material 8 -stage 0
updateMaterialStage -material 9 -stage 0
updateMaterialStage -material 10 -stage 0
updateMaterialStage -material 11 -stage 0
updateMaterialStage -material 12 -stage 0
updateMaterialStage -material 13 -stage 0
updateMaterialStage -material 14 -stage 0
updateMaterialStage -material 15 -stage 0

# 3.1 elastic gravity analysis (transient) 

constraints Transformation
test NormDispIncr 1.0e-4 35 1
algorithm   Newton
numberer RCM
system SparseGeneral
set gamma 8.33300000000000e-01
set beta 4.44400000000000e-01
integrator  Newmark $gamma $beta
analysis Transient

set startT  [clock seconds]
analyze     10 1.0
puts "Finished with elastic gravity analysis..."

# 3.2 plastic gravity analysis (transient)

updateMaterialStage -material 1 -stage 1
updateMaterialStage -material 2 -stage 1
updateMaterialStage -material 3 -stage 1
updateMaterialStage -material 4 -stage 1
updateMaterialStage -material 5 -stage 1
updateMaterialStage -material 6 -stage 1
updateMaterialStage -material 7 -stage 1
updateMaterialStage -material 8 -stage 1
updateMaterialStage -material 9 -stage 1
updateMaterialStage -material 10 -stage 1
updateMaterialStage -material 11 -stage 1
updateMaterialStage -material 12 -stage 1
updateMaterialStage -material 13 -stage 1
updateMaterialStage -material 14 -stage 1
updateMaterialStage -material 15 -stage 1



analyze     10 1.0
puts "Finished with plastic gravity analysis..."

# 3.3 Update element permeability for post gravity analysis

setParameter -value 1.019368e-08 -ele 1 hPerm 
setParameter -value 1.019368e-08 -ele 1 vPerm 
setParameter -value 1.019368e-08 -ele 2 hPerm 
setParameter -value 1.019368e-08 -ele 2 vPerm 
setParameter -value 1.019368e-08 -ele 3 hPerm 
setParameter -value 1.019368e-08 -ele 3 vPerm 
setParameter -value 1.019368e-08 -ele 4 hPerm 
setParameter -value 1.019368e-08 -ele 4 vPerm 
setParameter -value 1.019368e-08 -ele 5 hPerm 
setParameter -value 1.019368e-08 -ele 5 vPerm 
setParameter -value 1.019368e-08 -ele 6 hPerm 
setParameter -value 1.019368e-08 -ele 6 vPerm 
setParameter -value 1.019368e-08 -ele 7 hPerm 
setParameter -value 1.019368e-08 -ele 7 vPerm 
setParameter -value 1.019368e-08 -ele 8 hPerm 
setParameter -value 1.019368e-08 -ele 8 vPerm 
setParameter -value 1.019368e-08 -ele 9 hPerm 
setParameter -value 1.019368e-08 -ele 9 vPerm 
setParameter -value 1.019368e-08 -ele 10 hPerm 
setParameter -value 1.019368e-08 -ele 10 vPerm 
setParameter -value 1.019368e-08 -ele 11 hPerm 
setParameter -value 1.019368e-08 -ele 11 vPerm 
setParameter -value 1.019368e-08 -ele 12 hPerm 
setParameter -value 1.019368e-08 -ele 12 vPerm 
setParameter -value 1.019368e-08 -ele 13 hPerm 
setParameter -value 1.019368e-08 -ele 13 vPerm 
setParameter -value 1.019368e-08 -ele 14 hPerm 
setParameter -value 1.019368e-08 -ele 14 vPerm 
setParameter -value 1.019368e-08 -ele 15 hPerm 
setParameter -value 1.019368e-08 -ele 15 vPerm 



# ------------------------------------------------------------
# 4. Add the compliant base                                   
# ------------------------------------------------------------

# 4.1 Set basic properties of the base. 

set colThickness 1.000000e+00
set sElemX 2.500000e-01
set colArea [expr $sElemX*$colThickness]
set rockVs 1.800000e+02
set rockDen 2.000000e+00
set dashpotCoeff  [expr $rockVs*$rockDen]
uniaxialMaterial Viscous 2 [expr $dashpotCoeff*$colArea] 1
set cFactor [expr $colArea*$dashpotCoeff]


# 4.2 Create dashpot nodes and apply proper fixities. 

model BasicBuilder -ndm 2 -ndf 2

node 33 0.0 0.0
node 34 0.0 0.0
fix 33 1 1
fix 34 0 1

# 4.3 Apply equalDOF to the node connected to the column. 

equalDOF 1 34 1


# 4.4 Remove fixities created for gravity. 

remove sp 1 1
remove sp 2 1


# 4.5 Apply equalDOF for the first 4 nodes (3D) or 2 nodes (2D). 

equalDOF 1 2 1 


# 4.6 Create the dashpot element. 

element zeroLength 16 33 34 -mat 2  -dir 1



setTime 0.0
wipeAnalysis
remove recorders


# ------------------------------------------------------------
# 5. Dynamic analysis                                         
# ------------------------------------------------------------

model BasicBuilder -ndm 2 -ndf 3


# ------------------------------------------------------------
# 5.1 Apply the rock motion                                    
# ------------------------------------------------------------

set dT 1.000000e-03
set motionDT 5.000000e-03
set mSeries "Path -dt $motionDT -filePath Rock-x.vel -factor $cFactor"
pattern Plain 10 $mSeries {
    load 1  1.0 0.0 0.0
}

# ------------------------------------------------------------
# 5.2 Define the analysis                                     
# ------------------------------------------------------------

constraints Transformation
test NormDispIncr 1.0e-4 35 0
algorithm   Newton
numberer    RCM
system SparseGeneral
set gamma_dynm 5.000000e-01
set beta_dynm 2.500000e-01
integrator  Newmark $gamma_dynm $beta_dynm
set a0 7.870000e-01
set a1 7.942000e-04
rayleigh    $a0 $a1 0.0 0.0
analysis Transient

# ------------------------------------------------------------
# 5.3 Define outputs and recorders                            
# ------------------------------------------------------------

set recDT 1.000000e-03
file mkdir out_tcl
eval "recorder Node -file out_tcl/surface.disp -time -dT $recDT -node 32 -dof 1 2 3  disp"
eval "recorder Node -file out_tcl/surface.acc -time -dT $recDT -node 32 -dof 1 2 3  accel"
eval "recorder Node -file out_tcl/surface.vel -time -dT $recDT -node 32 -dof 1 2 3 vel"
eval "recorder Node -file out_tcl/base.disp -time -dT $recDT -node 1 -dof 1 2 3  disp"
eval "recorder Node -file out_tcl/base.acc -time -dT $recDT -node 1 -dof 1 2 3  accel"
eval "recorder Node -file out_tcl/base.vel -time -dT $recDT -node 1 -dof 1 2 3 vel"
eval "recorder Node -file out_tcl/displacement.out -time -dT $recDT -nodeRange 1 32 -dof 1 2  disp"
eval "recorder Node -file out_tcl/velocity.out -time -dT $recDT -nodeRange 1 32 -dof 1 2  vel"
eval "recorder Node -file out_tcl/acceleration.out -time -dT $recDT -nodeRange 1 32 -dof 1 2  accel"
eval "recorder Node -file out_tcl/porePressure.out -time -dT $recDT -nodeRange 1 32 -dof 3 vel"
recorder Element -file out_tcl/stress.out -time -dT $recDT  -eleRange 1 15  stress 3
recorder Element -file out_tcl/strain.out -time -dT $recDT  -eleRange 1 15  strain


# ------------------------------------------------------------
# 5.4 Perform dynamic analysis                                
# ------------------------------------------------------------

set nSteps 9994
set remStep 9994
set success 0

proc subStepAnalyze {dT subStep} {
	if {$subStep > 10} {
		return -10
	}
	for {set i 1} {$i < 3} {incr i} {
		puts "Try dT = $dT"
		set success [analyze 1 $dT]
		if {$success != 0} {
			set success [subStepAnalyze [expr $dT/2.0] [expr $subStep+1]]
			if {$success == -10} {
				puts "Did not converge."
				return $success
			}
		} else {
			if {$i==1} {
				puts "Substep $subStep : Left side converged with dT = $dT"
			} else {
				puts "Substep $subStep : Right side converged with dT = $dT"
			}
		}
	}
	return $success
}


puts "Start analysis"
set startT [clock seconds]
set finalTime [expr $remStep * $dT]
set success 0
set currentTime 0.
set timeMarker 0.
while {$success == 0 && $currentTime < $finalTime} {
	set subStep 0
	set success [analyze 1  $dT]
	if {$success != 0} {
	set curTime  [getTime]
	puts "Analysis failed at $curTime . Try substepping."
	set success  [subStepAnalyze [expr $dT/2.0] [incr subStep]]
	set curStep  [expr int($curTime/$dT + 1)]
	set remStep  [expr int($nSteps-$curStep)]
	puts "Current step: $curStep , Remaining steps: $remStep"
    } else {
          set progress [expr $currentTime/$finalTime * 100.]
          if { $progress > $timeMarker} {
              set timeMarker [expr $timeMarker+2]
              puts "$progress%"
              }
              set currentTime [getTime]
	}
}

set endT [clock seconds]

puts "loading analysis execution time: [expr $endT-$startT] seconds."

puts "Finished with dynamic analysis..."


wipe
puts "Site response analysis is finished."
exit

