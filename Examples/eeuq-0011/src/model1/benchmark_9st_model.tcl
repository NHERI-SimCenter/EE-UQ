#9-story benchmark structure
#All units converted to kips, ft

source Wsection.tcl; #tcl function for defining fibers of W sections(strong axis)


model basic -ndm 2 -ndf 3 # 2D frame with each node having 3 degrees of freedom

set eqScale 1;

set NStory 9; #Number of storeys
set NBay 5; #Number of bays

set LCol 13; #Height of Columns
set LBeam 30; #Length of Beams

node 1 0.0 0.0;
node 2 30.0 0.0;
node 3 60.0 0.0;
node 4 90.0 0.0;
node 5 120.0 0.0;
node 6 150.0 0.0;
node 7 0.0 12.0;
node 8 30.0 12.0;
node 9 60.0 12.0;
node 10 90.0 12.0;
node 11 120.0 12.0;
node 12 150.0 12.0;

# define NODAL COORDINATES-automated procedure
for {set level 1} {$level <=[expr $NStory]} {incr level 1} {
	set Y [expr ($level-1)*$LCol+12+18];
	for {set pier 1} {$pier <= [expr $NBay+1]} {incr pier 1} {
		set X [expr ($pier-1)*$LBeam];
		set nodeID [expr 12+$level*($NBay+1)+$pier-($NBay+1)]
		node $nodeID $X $Y;		# actually define node
	}}

#create additional nodes for moment releases

node 67 150.0 12.0;
node 68 150.0 30.0;
node 69 150.0 43.0;
node 70 150.0 56.0;
node 71 150.0 69.0;
node 72 150.0 82.0;
node 73 150.0 95.0;
node 74 150.0 108.0;
node 75 150.0 121.0;
node 76 150.0 134.0;
node 77 0.0 36.0;
node 78 30.0 36.0;
node 79 60.0 36.0;
node 80 90.0 36.0;
node 81 120.0 36.0;
node 82 150.0 36.0;
node 83 0.0 62.0;
node 84 30.0 62.0;
node 85 60.0 62.0;
node 86 90.0 62.0;
node 87 120.0 62.0;
node 88 150.0 62.0;
node 89 0.0 88.0;
node 90 30.0 88.0;
node 91 60.0 88.0;
node 92 90.0 88.0;
node 93 120.0 88.0;
node 94 150.0 88.0;
node 95 0.0 114.0;
node 96 30.0 114.0;
node 97 60.0 114.0;
node 98 90.0 114.0;
node 99 120.0 114.0;
node 100 150.0 114.0;

# BOUNDARY CONDITIONS
fixY 0.0 1 1 0;	# pin quake3traints for all Y=0.0 nodes
fix 7 1 0 0;
fix 12 1 0 0;

set intft 0.08333333333333;
set intfts [expr $intft*$intft];

# define MATERIAL properties
set Es [expr 29000/$intfts]; # Steel Young's Modulus
set nu 0.3;
set Gs [expr $Es/2./[expr 1+$nu]];  # Torsional stiffness Modulus

#Columns
set Fyc [expr 50/$intfts];
uniaxialMaterial SteelMPF 1 $Fyc $Fyc $Es 0.02 0.02 20.0 0.925 0.15

#Beams
set Fyb [expr 36/$intfts];
uniaxialMaterial SteelMPF 2 $Fyb $Fyb $Es 0.02 0.02 20.0 0.925 0.15


# ELEMENT properties
# Structural-Steel W-section properties

# section 1: W14x233
set d 1.3367;	# depth
set bf 1.3242;# flange width
set tf 0.1433;	# flange thickness
set tw 0.0892;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  1 1 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 2: W14x257
set d [expr 16.38*$intft];	# depth
set bf [expr 15.995*$intft];# flange width
set tf [expr 1.89*$intft];	# flange thickness
set tw [expr 1.175*$intft];	# web thickness
#set nfdw 16;		# number of fibers along dw
#set nftw 2;		# number of fibers along tw
#set nfbf 16;		# number of fibers along bf
#set nftf 4;		# number of fibers along tf
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  2 1 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 3: W14x283
set d 1.395;	# depth
set bf 1.3425;# flange width
set tf 0.1725;	# flange thickness
set tw 0.1075;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  3 1 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 4: W14x370
set d 1.4933;	# depth
set bf 1.3729;# flange width
set tf 0.2217;	# flange thickness
set tw 0.1379;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  4 1 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 5: W14x455
set d 1.585;	# depth
set bf 1.4029;# flange width
set tf 0.2675;	# flange thickness
set tw 0.1679;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  5 1 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 6: W14x500
set d 1.6333;	# depth
set bf 1.4175;# flange width
set tf 0.2917;	# flange thickness
set tw 0.1825;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  6 1 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 7: W24x68
set d [expr 23.73*$intft];	# depth
set bf [expr 8.965*$intft];# flange width
set tf [expr 0.585*$intft];	# flange thickness
set tw [expr 0.415*$intft];	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  7 2 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 8: W27x84
set d 2.2258;	# depth
set bf 0.83;    # flange width
set tf 0.0533;	# flange thickness
set tw 0.0383;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  8 2 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 9: W30x99
set d 2.4708;	# depth
set bf 0.8708;# flange width
set tf 0.0558;	# flange thickness
set tw 0.0433;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  9 2 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 10: W36x135
set d 2.9625;	# depth
set bf 0.9958;# flange width
set tf 0.0658;	# flange thickness
set tw 0.05;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  10 2 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# section 11: W36x160
set d 3.0008;	# depth
set bf 1.0;# flange width
set tf 0.085;	# flange thickness
set tw 0.0542;	# web thickness
set nfdw 8;		# number of fibers along dw
set nftw 1;		# number of fibers along tw
set nfbf 8;		# number of fibers along bf
set nftf 2;		# number of fibers along tf
Wsection  11 2 $d $bf $tf $tw $nfdw $nftw $nfbf $nftf

# define ELEMENTS
# set up geometric transformations of element
# separate columns and beams, in case of P-Delta analysis for columns
set IDColTransf 1; # all columns
set IDBeamTransf 2; # all beams
set ColTransfType Linear ;# options:Linear/PDelta/Corotational----you can change that
geomTransf $ColTransfType $IDColTransf  ;# only columns can have PDelta effects (gravity effects)
geomTransf Linear $IDBeamTransf

# Define Beam-Column Elements
set np 5; # number of Gauss integration points for nonlinear curvature distribution-- np=2 for linear distribution ok

# columns
element nonlinearBeamColumn 1 1 7 $np 4 $IDColTransf;		
element nonlinearBeamColumn 2 2 8 $np 6 $IDColTransf;
element nonlinearBeamColumn 3 3 9 $np 6 $IDColTransf;
element nonlinearBeamColumn 4 4 10 $np 6 $IDColTransf;
element nonlinearBeamColumn 5 5 11 $np 6 $IDColTransf;
element nonlinearBeamColumn 6 6 12 $np 4 $IDColTransf;
element nonlinearBeamColumn 7 7 13 $np 4 $IDColTransf;
element nonlinearBeamColumn 8 8 14 $np 6 $IDColTransf;
element nonlinearBeamColumn 9 9 15 $np 6 $IDColTransf;
element nonlinearBeamColumn 10 10 16 $np 6 $IDColTransf;
element nonlinearBeamColumn 11 11 17 $np 6 $IDColTransf;
element nonlinearBeamColumn 12 12 18 $np 4 $IDColTransf;
element nonlinearBeamColumn 13 13 77 $np 4 $IDColTransf;
element nonlinearBeamColumn 14 14 78 $np 6 $IDColTransf;
element nonlinearBeamColumn 15 15 79 $np 6 $IDColTransf;
element nonlinearBeamColumn 16 16 80 $np 6 $IDColTransf;
element nonlinearBeamColumn 17 17 81 $np 6 $IDColTransf;
element nonlinearBeamColumn 18 18 82 $np 4 $IDColTransf;
element nonlinearBeamColumn 19 77 19 $np 4 $IDColTransf;
element nonlinearBeamColumn 20 78 20 $np 5 $IDColTransf;
element nonlinearBeamColumn 21 79 21 $np 5 $IDColTransf;
element nonlinearBeamColumn 22 80 22 $np 5 $IDColTransf;
element nonlinearBeamColumn 23 81 23 $np 5 $IDColTransf;
element nonlinearBeamColumn 24 82 24 $np 4 $IDColTransf;
element nonlinearBeamColumn 25 19 25 $np 4 $IDColTransf;
element nonlinearBeamColumn 26 20 26 $np 5 $IDColTransf;
element nonlinearBeamColumn 27 21 27 $np 5 $IDColTransf;
element nonlinearBeamColumn 28 22 28 $np 5 $IDColTransf;
element nonlinearBeamColumn 29 23 29 $np 5 $IDColTransf;
element nonlinearBeamColumn 30 24 30 $np 4 $IDColTransf;
element nonlinearBeamColumn 31 25 83 $np 4 $IDColTransf;
element nonlinearBeamColumn 32 26 84 $np 5 $IDColTransf;
element nonlinearBeamColumn 33 27 85 $np 5 $IDColTransf;
element nonlinearBeamColumn 34 28 86 $np 5 $IDColTransf;
element nonlinearBeamColumn 35 29 87 $np 5 $IDColTransf;
element nonlinearBeamColumn 36 30 88 $np 4 $IDColTransf;
element nonlinearBeamColumn 37 83 31 $np 3 $IDColTransf;
element nonlinearBeamColumn 38 84 32 $np 4 $IDColTransf;
element nonlinearBeamColumn 39 85 33 $np 4 $IDColTransf;
element nonlinearBeamColumn 40 86 34 $np 4 $IDColTransf;
element nonlinearBeamColumn 41 87 35 $np 4 $IDColTransf;
element nonlinearBeamColumn 42 88 36 $np 3 $IDColTransf;
element nonlinearBeamColumn 43 31 37 $np 3 $IDColTransf;
element nonlinearBeamColumn 44 32 38 $np 4 $IDColTransf;
element nonlinearBeamColumn 45 33 39 $np 4 $IDColTransf;
element nonlinearBeamColumn 46 34 40 $np 4 $IDColTransf;
element nonlinearBeamColumn 47 35 41 $np 4 $IDColTransf;
element nonlinearBeamColumn 48 36 42 $np 3 $IDColTransf;
element nonlinearBeamColumn 49 37 89 $np 3 $IDColTransf;
element nonlinearBeamColumn 50 38 90 $np 4 $IDColTransf;
element nonlinearBeamColumn 51 39 91 $np 4 $IDColTransf;
element nonlinearBeamColumn 52 40 92 $np 4 $IDColTransf;
element nonlinearBeamColumn 53 41 93 $np 4 $IDColTransf;
element nonlinearBeamColumn 54 42 94 $np 3 $IDColTransf;
element nonlinearBeamColumn 55 89 43 $np 2 $IDColTransf;
element nonlinearBeamColumn 56 90 44 $np 3 $IDColTransf;
element nonlinearBeamColumn 57 91 45 $np 3 $IDColTransf;
element nonlinearBeamColumn 58 92 46 $np 3 $IDColTransf;
element nonlinearBeamColumn 59 93 47 $np 3 $IDColTransf;
element nonlinearBeamColumn 60 94 48 $np 2 $IDColTransf;
element nonlinearBeamColumn 61 43 49 $np 2 $IDColTransf;
element nonlinearBeamColumn 62 44 50 $np 3 $IDColTransf;
element nonlinearBeamColumn 63 45 51 $np 3 $IDColTransf;
element nonlinearBeamColumn 64 46 52 $np 3 $IDColTransf;
element nonlinearBeamColumn 65 47 53 $np 3 $IDColTransf;
element nonlinearBeamColumn 66 48 54 $np 2 $IDColTransf;
element nonlinearBeamColumn 67 49 95 $np 2 $IDColTransf;
element nonlinearBeamColumn 68 50 96 $np 3 $IDColTransf;
element nonlinearBeamColumn 69 51 97 $np 3 $IDColTransf;
element nonlinearBeamColumn 70 52 98 $np 3 $IDColTransf;
element nonlinearBeamColumn 71 53 99 $np 3 $IDColTransf;
element nonlinearBeamColumn 72 54 100 $np 2 $IDColTransf;
element nonlinearBeamColumn 73 95 55 $np 1 $IDColTransf;
element nonlinearBeamColumn 74 96 56 $np 2 $IDColTransf;
element nonlinearBeamColumn 75 97 57 $np 2 $IDColTransf;
element nonlinearBeamColumn 76 98 58 $np 2 $IDColTransf;
element nonlinearBeamColumn 77 99 59 $np 2 $IDColTransf;
element nonlinearBeamColumn 78 100 60 $np 1 $IDColTransf;
element nonlinearBeamColumn 79 55 61 $np 1 $IDColTransf;
element nonlinearBeamColumn 80 56 62 $np 2 $IDColTransf;
element nonlinearBeamColumn 81 57 63 $np 2 $IDColTransf;
element nonlinearBeamColumn 82 58 64 $np 2 $IDColTransf;
element nonlinearBeamColumn 83 59 65 $np 2 $IDColTransf;
element nonlinearBeamColumn 84 60 66 $np 1 $IDColTransf;

#beams
element nonlinearBeamColumn 85 7 8 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 86 8 9 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 87 9 10 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 88 10 11 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 89 11 67 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 90 13 14 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 91 14 15 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 92 15 16 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 93 16 17 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 94 17 68 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 95 19 20 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 96 20 21 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 97 21 22 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 98 22 23 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 99 23 69 $np 11 $IDBeamTransf;
element nonlinearBeamColumn 100 25 26 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 101 26 27 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 102 27 28 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 103 28 29 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 104 29 70 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 105 31 32 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 106 32 33 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 107 33 34 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 108 34 35 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 109 35 71 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 110 37 38 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 111 38 39 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 112 39 40 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 113 40 41 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 114 41 72 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 115 43 44 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 116 44 45 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 117 45 46 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 118 46 47 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 119 47 73 $np 10 $IDBeamTransf;
element nonlinearBeamColumn 120 49 50 $np 9 $IDBeamTransf;
element nonlinearBeamColumn 121 50 51 $np 9 $IDBeamTransf;
element nonlinearBeamColumn 122 51 52 $np 9 $IDBeamTransf;
element nonlinearBeamColumn 123 52 53 $np 9 $IDBeamTransf;
element nonlinearBeamColumn 124 53 74 $np 9 $IDBeamTransf;
element nonlinearBeamColumn 125 55 56 $np 8 $IDBeamTransf;
element nonlinearBeamColumn 126 56 57 $np 8 $IDBeamTransf;
element nonlinearBeamColumn 127 57 58 $np 8 $IDBeamTransf;
element nonlinearBeamColumn 128 58 59 $np 8 $IDBeamTransf;
element nonlinearBeamColumn 129 59 75 $np 8 $IDBeamTransf;
element nonlinearBeamColumn 130 61 62 $np 7 $IDBeamTransf;
element nonlinearBeamColumn 131 62 63 $np 7 $IDBeamTransf;
element nonlinearBeamColumn 132 63 64 $np 7 $IDBeamTransf;
element nonlinearBeamColumn 133 64 65 $np 7 $IDBeamTransf;
element nonlinearBeamColumn 134 65 76 $np 7 $IDBeamTransf;

#define masses(lumped masses per floor)
mass 13 34.5 1e-15 0.;
mass 19 33.85 1e-15 0.;
mass 25 33.85 1e-15 0.;
mass 31 33.85 1e-15 0.;
mass 37 33.85 1e-15 0.;
mass 43 33.85 1e-15 0.;
mass 49 33.85 1e-15 0.;
mass 55 33.85 1e-15 0.;
mass 61 36.6 1e-15 0.;

#equal constraint---------moment releases 
equalDOF 12 67 1 2;
equalDOF 18 68 1 2;
equalDOF 24 69 1 2;
equalDOF 30 70 1 2;
equalDOF 36 71 1 2;
equalDOF 42 72 1 2;
equalDOF 48 73 1 2;
equalDOF 54 74 1 2;
equalDOF 60 75 1 2;
equalDOF 66 76 1 2;

#equal constraint---------diaphragm constraint 
equalDOF 12 8 1;
equalDOF 12 9 1;
equalDOF 12 10 1;
equalDOF 12 11 1;
equalDOF 18 13 1;
equalDOF 18 14 1;
equalDOF 18 15 1;
equalDOF 18 16 1;
equalDOF 18 17 1;
equalDOF 24 19 1;
equalDOF 24 20 1;
equalDOF 24 21 1;
equalDOF 24 22 1;
equalDOF 24 23 1;
equalDOF 30 25 1;
equalDOF 30 26 1;
equalDOF 30 27 1;
equalDOF 30 28 1;
equalDOF 30 29 1;
equalDOF 36 31 1;
equalDOF 36 32 1;
equalDOF 36 33 1;
equalDOF 36 34 1;
equalDOF 36 35 1;
equalDOF 42 37 1;
equalDOF 42 38 1;
equalDOF 42 39 1;
equalDOF 42 40 1;
equalDOF 42 41 1;
equalDOF 48 43 1;
equalDOF 48 44 1;
equalDOF 48 45 1;
equalDOF 48 46 1;
equalDOF 48 47 1;
equalDOF 54 49 1;
equalDOF 54 50 1;
equalDOF 54 51 1;
equalDOF 54 52 1;
equalDOF 54 53 1;
equalDOF 60 55 1;
equalDOF 60 56 1;
equalDOF 60 57 1;
equalDOF 60 58 1;
equalDOF 60 59 1;
equalDOF 66 61 1;
equalDOF 66 62 1;
equalDOF 66 63 1;
equalDOF 66 64 1;
equalDOF 66 65 1;

system FullGeneral

#extract stiffness matrix------------------------------------------------------
test NormDispIncr 1.0e-6    6     0
algorithm  Newton
system FullGeneral
constraints Transformation
integrator LoadControl 1.0
numberer Plain
analysis Static
analyze 1 
# printA -file "kMatrix.out"
loadConst -time 0.0

#eigenvalues
set eigenvalues [eigen 9] 
set omega {}
set f {}
set T {}
set pi 3.141593

puts " "
puts "PERIODS (sec):"

foreach lam $eigenvalues {
lappend omega [expr sqrt($lam)]
lappend f [expr sqrt($lam)/(2*$pi)]
lappend T [expr (2*$pi)/sqrt($lam)]
puts "[expr (2*$pi)/sqrt($lam)]"
}

# Define RECORDERS 
# recorder Node -file quake3/Disp1.out -time -node 13 -dof 1 disp;
# recorder Node -file quake3/Disp2.out -time -node 19 -dof 1 disp;
# recorder Node -file quake3/Disp3.out -time -node 25 -dof 1 disp;
# recorder Node -file quake3/Disp4.out -time -node 31 -dof 1 disp;
# recorder Node -file quake3/Disp5.out -time -node 37 -dof 1 disp;
# recorder Node -file quake3/Disp6.out -time -node 43 -dof 1 disp;
# recorder Node -file quake3/Disp7.out -time -node 49 -dof 1 disp;
# recorder Node -file quake3/Disp8.out -time -node 55 -dof 1 disp;
# recorder Node -file quake3/Disp9.out -time -node 61 -dof 1 disp;

# recorder Node -file quake3/Accel1.out -time -node 13 -dof 1 accel;
# recorder Node -file quake3/Accel2.out -time -node 19 -dof 1 accel;
# recorder Node -file quake3/Accel3.out -time -node 25 -dof 1 accel;
# recorder Node -file quake3/Accel4.out -time -node 31 -dof 1 accel;
# recorder Node -file quake3/Accel5.out -time -node 37 -dof 1 accel;
# recorder Node -file quake3/Accel6.out -time -node 43 -dof 1 accel;
# recorder Node -file quake3/Accel7.out -time -node 49 -dof 1 accel;
# recorder Node -file quake3/Accel8.out -time -node 55 -dof 1 accel;
# recorder Node -file quake3/Accel9.out -time -node 61 -dof 1 accel;


# recorder Drift -file quake3/Drift1.out -time -iNode 7 -jNode 13 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift2.out -time -iNode 13 -jNode 19 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift3.out -time -iNode 19 -jNode 25 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift4.out -time -iNode 25 -jNode 31 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift5.out -time -iNode 31 -jNode 37 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift6.out -time -iNode 37 -jNode 43 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift7.out -time -iNode 43 -jNode 49 -dof 1 -perpDirn 2;
# recorder Drift -file quake3/Drift8.out -time -iNode 49 -jNode 55 -dof 1 -perpDirn 2;
# # recorder Drift -file quake3/Drift9.out -time -iNode 55 -jNode 61 -dof 1 -perpDirn 2;

# recorder EnvelopeNode -file disp.out -node 13 19 25 31 37 43 49 55 61 -dof 1 disp
# recorder EnvelopeNode -file acc.out -node 13 19 25 31 37 43 49 55 61 -dof 1  accel

# #Time-History Analysis--------------------------------------------------------------------------------------------------------------------

# # Rayleigh Damping
# # calculate damping parameters
# set zeta   0.0200;
# set wi [lindex $omega [expr 0]];
# set wj [lindex $omega [expr 2]];    # select modes of critical damping for calculatinf Rayleigh coefficients
# set a0 [expr $zeta*2.0*$wi*$wj/($wi + $wj)];	# mass damping coefficient based on first and third mode
# set a1 [expr $zeta*2.0/($wi + $wj)];		# stiffness damping coefficient based on first and third mode
# # assign damping to frame		
# rayleigh $a0 0.0 $a1 0.0; # assign mass proportional damping to structure

# # define ground motion parameters
# set patternID 1;				  # load pattern ID
# set GMdirection 1;				  # ground motion direction (1 = x)
# set GMfile "quakeFiles/quake3.txt";		      # ground motion filename

# set dt    0.010
# set Scalefact    [expr 0.033*$eqScale]
# set GMtime 39.870
# # define the acceleration series for the ground motion
# # syntax:  "Series -dt $timestep_of_record -filePath $filename_with_acc_history -factor $scale_record_by_this_amount
# set accelSeries "Series -dt $dt -filePath $GMfile -factor $Scalefact";
 
# # create load pattern: apply acceleration to all fixed nodes with UniformExcitation
# # command: pattern UniformExcitation $patternID $GMdir -accel $timeSeriesID 
# pattern UniformExcitation $patternID $GMdirection -accel $accelSeries;

# # define dynamic analysis parameters
# set dt_analysis 0.01;			          # timestep of analysis
# wipeAnalysis;					          # destroy all components of the Analysis object, i.e. any objects created with system, numberer, constraints, integrator, algorithm, and analysis commands
# constraints Plain;				          # how it handles boundary conditions
# numberer RCM;					          # renumber dof's to minimize band-width (optimization)
# system UmfPack;		 		              # how to store and solve the system of equations in the analysis
# test NormDispIncr 1.0e-8 50;	          # type of convergence criteria with tolerance, max iterations
# algorithm NewtonLineSearch;		          # use NewtonLineSearch solution algorithm: updates tangent stiffness at every iteration and introduces line search to the Newton-Raphson algorithm to solve the nonlinear quake3idual equation. Line search increases the effectiveness of the Newton method
# integrator Newmark 0.5 0.25;	          # uses Newmark's average acceleration method to compute the time history
# analysis Transient;				          # type of analysis: transient or static
# set NumSteps [expr round(($GMtime + 0.0)/$dt_analysis)];  # number of steps in analysis

# # perform the dynamic analysis and display whether analysis was successful	
# set ok [analyze $NumSteps $dt_analysis];	# ok = 0 if analysis was completed
# if {$ok == 0} {
#          puts "Dynamic analysis complete";
# } else {
# 	 puts "Dynamic analysis did not converge";
# }		
 
# # output time at end of analysis	
# set currentTime [getTime];	# get current analysis time	(after dynamic analysis)
# puts "The current time is: $currentTime";

# print info # creates a file with nodal and element information

# wipe all;

