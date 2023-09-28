# Reduced Order Model for 9 story steel structure
# units: kips, ft, sec
# Created by: Aakash Bangalore Satish
# Last modified on: May 2 2022

set t1 [clock milliseconds]
#set eqScale 1
# parameters to be calibrated
set delta_y_1 0.145661
set delta_y_2 0.073379
set delta_y_3 0.088417
set delta_y_4 0.119957
set delta_y_5 0.079751
set delta_y_6 0.137673
set delta_y_7 0.111261
set delta_y_8 0.106056
set delta_y_9 0.140737
set alpha_1 0.590509
set alpha_2 0.800973
set alpha_3 0.687800
set alpha_4 0.817615
set alpha_5 0.780905
set alpha_6 0.630119
set alpha_7 0.760197
set alpha_8 0.781489
set alpha_9 0.673541
set nu_1 1.899878
set nu_2 1.986417
set nu_3 1.957681
set nu_4 1.655571
set nu_5 2.229184
set nu_6 2.040811
set nu_7 2.276190
set nu_8 2.299513
set nu_9 1.812283
set rho_1 0.229556
set rho_2 0.307698
set rho_3 0.276846
set rho_4 0.393603
set rho_5 0.438462
set rho_6 0.416562
set rho_7 0.764021
set rho_8 0.505961
set rho_9 0.301157 

set n_1 $nu_1 
set n_2 $nu_2 
set n_3 $nu_3 
set n_4 $nu_4 
set n_5 $nu_5 
set n_6 $nu_6 
set n_7 $nu_7 
set n_8 $nu_8 
set n_9 $nu_9  

set beta_1 [expr $rho_1/pow($delta_y_1, $nu_1)]
set beta_2 [expr $rho_2/pow($delta_y_2, $nu_2)]
set beta_3 [expr $rho_3/pow($delta_y_3, $nu_3)]
set beta_4 [expr $rho_4/pow($delta_y_4, $nu_4)]
set beta_5 [expr $rho_5/pow($delta_y_5, $nu_5)]
set beta_6 [expr $rho_6/pow($delta_y_6, $nu_6)]
set beta_7 [expr $rho_7/pow($delta_y_7, $nu_7)]
set beta_8 [expr $rho_8/pow($delta_y_8, $nu_8)]
set beta_9 [expr $rho_9/pow($delta_y_9, $nu_9)]

set gamma_1 [expr (1-$rho_1)/pow($delta_y_1, $nu_1)]
set gamma_2 [expr (1-$rho_2)/pow($delta_y_2, $nu_2)]
set gamma_3 [expr (1-$rho_3)/pow($delta_y_3, $nu_3)]
set gamma_4 [expr (1-$rho_4)/pow($delta_y_4, $nu_4)]
set gamma_5 [expr (1-$rho_5)/pow($delta_y_5, $nu_5)]
set gamma_6 [expr (1-$rho_6)/pow($delta_y_6, $nu_6)]
set gamma_7 [expr (1-$rho_7)/pow($delta_y_7, $nu_7)]
set gamma_8 [expr (1-$rho_8)/pow($delta_y_8, $nu_8)]
set gamma_9 [expr (1-$rho_9)/pow($delta_y_9, $nu_9)]

# Parameters
set nDOF 9
set Lc 0

# set GMNumStepsList {6000 5278 3987 3989 3631 3947 4089}
# set scenarioQuakesList {3}
# set include_quakes_list {3}
set printPeriods 0
# for {set i 0} {$i < [llength $scenarioQuakesList]} {incr i} {
#     set quakeNum [lindex $scenarioQuakesList $i]
#     if {$quakeNum in $include_quakes_list} {
        # puts "Running analysis for quake $i"
        puts " "
        # source nodes, materials, and elements files
        wipe
        model Basic -ndm 2 -ndf 3
        source Nodes.tcl
        source Materials.tcl
        source Elements.tcl

        # Eigenvalue analysis for Rayleigh damping
        set numMode 27
        set lambda [eigen -fullGenLapack $numMode]

        if {$printPeriods==0} {
            set omega {}
            puts "PERIODS (sec):"
            set pi [expr {2.0*asin(1.0)}]
            foreach lam $lambda {
                if {$lam != 0} {
                    lappend omega [expr sqrt($lam)]
                    lappend f [expr sqrt($lam)/(2*$pi)]
                    lappend T [expr (2*$pi)/sqrt($lam)]
                    puts "[expr (2*$pi)/sqrt($lam)] w: [expr sqrt($lam)]"
                }
            }
            puts " "
            set printPeriods 1
        }

        # set mode1 1
        # set mode2 2
        # set pDamp 0.02
        # set w1 [lindex $omega [expr $mode1 -1]]
        # set w2 [lindex $omega [expr $mode2 -1]]
        # set alphaM [expr $pDamp*2*$w1*$w2/($w1+$w2)]
        # set betaK  [expr $pDamp*2/($w1+$w2)]
        # rayleigh $alphaM $betaK 0.0 0.0

        # # Read in the ground motion, create series, define ground motion
        # set dt 0.01
        # set GMtime [expr [lindex $GMNumStepsList [expr $quakeNum-1]]*$dt]
        # set numStep [lindex $GMNumStepsList [expr $quakeNum-1]]
        # set GMfile "./quakeFiles/quake$quakeNum.txt"
        # set patternID 1
        # set GMdirection 1
        # set Scalefact    [expr 0.033]
        # set accelSeries "Series -dt $dt -filePath $GMfile -factor $Scalefact"
        # pattern UniformExcitation $patternID $GMdirection -accel $accelSeries

        # # Define RECORDERS 
        # set dataDir quake$quakeNum
        # # file mkdir $dataDir
        # # recorder Drift -file "$dataDir/Drift1.out" -time -iNode 10 -jNode 1 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift2.out" -time -iNode 1 -jNode 2 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift3.out" -time -iNode 2 -jNode 3 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift4.out" -time -iNode 3 -jNode 4 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift5.out" -time -iNode 4 -jNode 5 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift6.out" -time -iNode 5 -jNode 6 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift7.out" -time -iNode 6 -jNode 7 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift8.out" -time -iNode 7 -jNode 8 -dof 1 -perpDirn 2
        # # recorder Drift -file "$dataDir/Drift9.out" -time -iNode 8 -jNode 9 -dof 1 -perpDirn 2


        # recorder EnvelopeNode -file disp.out -node 1 2 3 4 5 6 7 8 9 -dof 1 disp
        # recorder EnvelopeNode -file acc.out -node 1 2 3 4 5 6 7 8 9  -dof 1  accel

        # # Define analysis parameters
        # set gamma 0.5
        # set beta 0.25
        # set Tol 1e-8
        # set testType NormDispIncr
        # set numTestStep 50
        # set algorithmType NewtonLineSearch

        # system FullGeneral
        # constraints Plain
        # test $testType $Tol $numTestStep
        # algorithm $algorithmType
        # numberer Plain
        # integrator Newmark $gamma $beta
        # analysis Transient

        # # Perform analysis
        # set tFinal [expr $GMtime]
        # set tCurrent [getTime]
        # set ok 0
        # while {$ok == 0 && $tCurrent < $tFinal} {
        #     set ok [analyze 1 $dt]
        #     set tCurrent [getTime]
        # }
        # wipe analysis
    # }

# }

set t2 [clock milliseconds]
puts "Analysis finished in [expr ($t2 - $t1)/1000.0] seconds"