if {$pid == 0} {puts [string repeat "=" 120] }
if {$pid == 0} {puts "Starting analysis : DynamicAnalysis"}
if {$pid == 0} {puts [string repeat "=" 120] }
constraints Plain
numberer ParallelRCM
system Mumps -ICNTL14 200 -ICNTL7 7
algorithm ModifiedNewton -factoronce
test EnergyIncr 0.0001 10 5
integrator Newmark 0.5 0.25
analysis Transient
while {[getTime] < 20.0} {
	if {$pid == 0} {puts "Time : [getTime]"}

	set Ok [analyze 1 0.01]

}
wipeAnalysis