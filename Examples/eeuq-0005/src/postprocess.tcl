
set nodeIn [open node.out r]
while { [gets $nodeIn data] >= 0 } {
     set maxDisplacement $data
}
puts $maxDisplacement

# create file handler to write results to output & list into which we will put results
set resultFile [open results.out w]
set results []

# for each quanity in list of QoI passed
#  - get nodeTag
#  - get nodal displacement if valid node, output 0.0 if not
#  - for valid node output displacement, note if dof not provided output 1'st dof

foreach edp $listQoI {
	set splitEDP [split $edp "_"]	
	set nodeTag [lindex $splitEDP 1]
        if {[llength $splitEDP] == 3} {
           set dof 1
	} else {
            set dof [lindex $splitEDP 3]
	} 
    set nodeDisp [lindex $maxDisplacement [expr (($nodeTag-1)*2)+$dof-1]]
	lappend results $nodeDisp
}

# send results to output file & close the file
puts $resultFile $results
close $resultFile
