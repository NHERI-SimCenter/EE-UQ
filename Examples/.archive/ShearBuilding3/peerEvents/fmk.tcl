foreach {motion fact} {elCentro 1.5 Rinaldi228 1.0 Rinaldi318 1.0 elCentro 1.0} {
    wipe
    source ../ShearBuilding3.tcl
    source ReadRecord.tcl
    
    set dT 1.0
    set nPts 1
    ReadRecord $motion.AT2  motion.dat dT nPts
    timeSeries Path 1 -dt $dT -filePath motion.dat -factor [expr $g*$fact]
    pattern UniformExcitation 1 1 -accel 1
    
    recorder EnvelopeNode -file node.out -node 4 -dof 1 disp
    
    numberer Plain
    constraints Plain
    system Umfpack
    test NormUnbalance 1.0e-6 10
    algorithm Newton
    integrator Newmark 0.5 0.25; #[expr 1./6.]
    analysis Transient
    
    set eig [eigen 3]
    
    modalDamping 0.02
    analyze $nPts $dT

    wipe
    set file [open node.out r]
    while { [gets $file data] >= 0 } {
     set res $data
    }
    close $file
    puts "motion: $motion $res"
}


