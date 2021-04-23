# --------------------------------------------------------------------------------------------------
# Example: 4-Story Steel Moment Frame with Concentrated Plasticity
# Centerline Model with Concentrated Plastic Hinges at Beam-Column Joint
# Created by:  Laura Eads and Kuanshi Zhong, Stanford University, 2021
# Units: kips, inches, seconds

# Updated 9 May 2012:  fixed errors defining rayleigh damping (see line 440)
# Updated 3 Sept 2013: changed solution algorithm and convergence criteria to help with convergence (by Filipe Ribeiro and Andre Barbosa) 
# Updated 21 Feb 2021: changed to a 4-story frame

# Element and Node ID conventions:
#	1xy = frame columns with springs at both ends
#	2xy = frame beams with springs at both ends
#	6xy = trusses linking frame and P-delta column
#	7xy = P-delta columns
#	3,xya = frame column rotational springs
#	4,xya = frame beam rotational springs
#	5,xya = P-delta column rotational springs
#	where:
#		x = Pier or Bay #
#		y = Floor or Story #
#		a = an integer describing the location relative to beam-column joint (see description where elements and nodes are defined)
	
############################################################################
#   Time History/Dynamic Analysis               			   			   #
############################################################################	
	
	# Rayleigh Damping
		# calculate damping parameters
		set zeta 0.02;		# percentage of critical damping
		set a0 [expr $zeta*2.0*$w1*$w2/($w1 + $w2)];	# mass damping coefficient based on first and second modes
		set a1 [expr $zeta*2.0/($w1 + $w2)];			# stiffness damping coefficient based on first and second modes
		set a1_mod [expr $a1*(1.0+$n)/$n];				# modified stiffness damping coefficient used for n modified elements. See Zareian & Medina 2010.
		
		# assign damping to frame beams and columns		
		# command: region $regionID -eleRange $elementIDfirst $elementIDlast -rayleigh $alpha_mass $alpha_currentStiff $alpha_initialStiff $alpha_committedStiff
		## old commands:  region 4 -eleRange 111 222 rayleigh 0.0 0.0 $a1_mod 0.0;	# assign stiffness proportional damping to frame beams & columns w/ n modifications
		##				  rayleigh $a0 0.0 0.0 0.0;              					# assign mass proportional damping to structure (only assigns to nodes with mass)
		## updated 9 May 2012:	use "-rayleigh" instead of "rayleigh" when defining damping with the "region" command
		## 						use "region" command when defining mass proportional damping so that the stiffness proportional damping isn't canceled
		region 4 -eleRange 111 245 -rayleigh 0.0 0.0 $a1_mod 0.0;	# assign stiffness proportional damping to frame beams & columns w/ n modifications
		region 5 -node 12 13 14 15 22 23 24 25 32 33 34 35 42 43 44 45 51 52 53 54 55 -rayleigh $a0 0.0 0.0 0.0;		# assign mass proportional damping to structure (assign to nodes with mass)
		
	# define dynamic analysis parameters
		wipeAnalysis;					# destroy all components of the Analysis object, i.e. any objects created with system, numberer, constraints, integrator, algorithm, and analysis commands
		constraints Plain;				# how it handles boundary conditions
		numberer RCM;					# renumber dof's to minimize band-width (optimization)
		system UmfPack;					# how to store and solve the system of equations in the analysis
		test NormDispIncr 1.0e-6 50;	# type of convergence criteria with tolerance, max iterations
		algorithm NewtonLineSearch;		# use NewtonLineSearch solution algorithm: updates tangent stiffness at every iteration and introduces line search to the Newton-Raphson algorithm to solve the nonlinear residual equation. Line search increases the effectiveness of the Newton method
		integrator Newmark 0.5 0.25;	# uses Newmark's average acceleration method to compute the time history
		analysis Transient;				# type of analysis: transient or static
		
	# perform the dynamic analysis and display whether analysis was successful	
		set dtAna [expr $dt/2]
		set dtMin 1.0e-9
		set dtMinex 1.0e-12
		set dtMax $dtAna

		set ok 0;
		set tFinal [expr $numStep * $dt]
		set tCurrent [getTime]

		puts "Start time history analysis";

		record;

		set algo_tag 0;
		set timer	1;
		while {$ok == 0 && $tCurrent < $tFinal} {
			
			set ok [analyze 1 $dtAna]
			
			if {$ok != 0} {
				if {[expr $dtAna/2.0] >= $dtMin} {
					set dtAna [expr $dtAna/2.0]
					puts [format "\nReducing time step size (dtNew = %1.6e)" $dtAna]
					set ok 0;
				} else {
				if {[expr $dtAna/2.0] >= $dtMinex} {
					set dtAna [expr $dtAna/2.0]
					algorithm KrylovNewton
					puts "Try KrylovNewton"
					set algo_tag 1;
					set ok 0
				}
			}
			} else {
				if {[expr $dtAna*2.0] <= $dtMax} {
				if {$algo_tag == 1} {
					puts "Back to Newton"
					algorithm Newton
					set algo_tag 0
				}
					set dtAna [expr $dtAna*2.0]
					puts [format "\nIncreasing time step size (dtNew = %1.6e)" $dtAna]
				}
			}
			
			set tCurrent [getTime]
		}

		if {$ok != 0} {
			puts [format "\nModel failed (time = %1.3e)" $tCurrent]
		} else {
			puts [format "\nResponse-history analysis completed"]
		}