if {$pid == 0} {puts [string repeat "=" 120] }
if {$pid == 0} {puts "Starting analysis : DynamicAnalysis"}
if {$pid == 0} {puts [string repeat "=" 120] }
constraints Penalty 1.e12 1.e12
numberer ParallelRCM
system Mumps -ICNTL14 400 -ICNTL7 7
algorithm ModifiedNewton -factoronce
test NormDispIncr 0.001 5 2 2
integrator Newmark 0.5 0.25
analysis Transient
while {[getTime] < 32.0} {
	if {$pid == 0} {puts "Time : [getTime]"}

	set Ok [analyze 1 0.01]

}
wipeAnalysis