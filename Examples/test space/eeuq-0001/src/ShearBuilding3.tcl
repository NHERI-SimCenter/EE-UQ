#units kips/in

# some parameters
# pset is used for set up random variables
pset w 100.0
pset wR 50.0
pset k 326.32

set E 29000.0
set A 1e4
set Ic [expr $k*144*144*144/(24.*$E)]
set Ib 1e12
set g 386.1

puts [expr $w/$g]
puts [expr $wR/$g]

# the model
model BasicBuilder -ndm 2 -ndf 3
# create node
# node $nodeTag $X $Y <-mass $massX $massY $massR>
node 1    0.   0.0 
node 2    0. 144.0 -mass [expr $w/$g] [expr $w/$g]     0.
node 3    0. 288.0 -mass [expr $w/$g] [expr $w/$g]     0.
node 4    0. 432.0 -mass [expr $wR/$g] [expr $wR/$g]   0.
node 5  288.   0.0 
node 6  288. 144.0 
node 7  288. 288.0 
node 8  288. 432.0 
# boundary conditions
# fix $nodeTag $bcX $bcY $bcR
fix 1 1 1 1
fix 5 1 1 1
# constrain nodal DOFs
# equalDOF $nodeTag1 $nodeTag2 $DOF
equalDOF 2 6 1
equalDOF 3 7 1
equalDOF 4 8 1
# geometric transformation
# geomTraf $type $tranfTag
geomTransf Linear 1
# element elasticBeamColumn $elementTag $nodeTag1 $nodeTag2 $area $momentOfInertia $modulus $tranfTag
element elasticBeamColumn 1 1 2 $A $Ic $E 1
element elasticBeamColumn 2 2 3 $A $Ic $E 1
element elasticBeamColumn 3 3 4 $A $Ic $E 1
element elasticBeamColumn 4 5 6 $A $Ic $E 1
element elasticBeamColumn 5 6 7 $A $Ic $E 1
element elasticBeamColumn 6 7 8 $A $Ic $E 1
element elasticBeamColumn 7 2 6 $A $Ib $E 1
element elasticBeamColumn 8 3 7 $A $Ib $E 1
element elasticBeamColumn 9 4 8 $A $Ib $E 1

