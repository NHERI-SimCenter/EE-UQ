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

###################################################################################################
#          Set Up & Source Definition									  
###################################################################################################
	wipe all;							# clear memory of past model definitions
	model BasicBuilder -ndm 2 -ndf 3;	# Define the model builder, ndm = #dimension, ndf = #dofs
	#source rotSpring2DModIKModel.tcl;	# procedure for defining a rotational spring (zero-length element)
	#source rotLeaningCol.tcl;			# procedure for defining a rotational spring (zero-length element) with very small stiffness

proc rotLeaningCol {eleID nodeR nodeC} {

	#Spring Stiffness
	set K 1e-9; # k/in

	# Create the material and zero length element (spring)
    uniaxialMaterial Elastic  $eleID  $K	
	element zeroLength $eleID $nodeR $nodeC -mat $eleID -dir 6

	# Constrain the translational DOF with a multi-point constraint	
	#   		retained constrained DOF_1 DOF_2 
	equalDOF    $nodeR     $nodeC     1     2
}

proc rotSpring2DModIKModel {eleID nodeR nodeC K asPos asNeg MyPos MyNeg LS LK LA LD cS cK cA cD th_pP th_pN th_pcP th_pcN ResP ResN th_uP th_uN DP DN} {
#
# Create the zero length element
      uniaxialMaterial Bilin  $eleID  $K  $asPos $asNeg $MyPos $MyNeg $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

      element zeroLength $eleID $nodeR $nodeC -mat $eleID -dir 6

# Constrain the translational DOF with a multi-point constraint
#          			retained constrained DOF_1 DOF_2 ... DOF_n
           equalDOF    $nodeR     $nodeC     1     2
}
	
###################################################################################################
#          Define Building Geometry, Nodes, and Constraints											  
###################################################################################################
# define structure-geometry parameters
	set NStories 4;						# number of stories
	set NBays 4;						# number of frame bays (excludes bay for P-delta column)
	set WBay      [expr 30.0*12.0];		# bay width in inches
	set HStory1   [expr 13.0*12.0];		# 1st story height in inches
	set HStoryTyp [expr 13.0*12.0];		# story height of other stories in inches
	set HBuilding [expr $HStory1 + ($NStories-1)*$HStoryTyp];	# height of building

# calculate locations of beam/column joints:
	set Pier1  0.0;		# leftmost column line
	set Pier2  [expr $Pier1 + $WBay];
	set Pier3  [expr $Pier2 + $WBay];
	set Pier4  [expr $Pier3 + $WBay];
	set Pier5  [expr $Pier4 + $WBay];
	set Pier6  [expr $Pier5 + $WBay];	# P-delta column line
	set Floor1 0.0;		# ground floor
	set Floor2 [expr $Floor1 + $HStory1];
	set Floor3 [expr $Floor2 + $HStoryTyp];
	set Floor4 [expr $Floor3 + $HStoryTyp];
	set Floor5 [expr $Floor4 + $HStoryTyp];

# calculate joint offset distance for beam plastic hinges
	set phlat23 [expr 0.0];		# lateral dist from beam-col joint to loc of hinge on Floor 2

# calculate nodal masses -- lump floor masses at frame nodes
	set g 386.2;				# acceleration due to gravity
	set Floor2Weight 1014.74;		# weight of Floor 2 in kips
	set Floor3Weight 1014.74;		# weight of Floor 3 in kips
	set Floor4Weight 1014.74;		# weight of Floor 2 in kips
	set Floor5Weight 1014.74;		# weight of Floor 3 in kips
	set WBuilding  [expr $Floor2Weight + $Floor3Weight + $Floor4Weight + $Floor5Weight];# total building weight
	set NodalMass2 [expr ($Floor2Weight/$g) / (5.0)];	# mass at each node on Floor 2
	set NodalMass3 [expr ($Floor3Weight/$g) / (5.0)];	# mass at each node on Floor 3
	set NodalMass4 [expr ($Floor4Weight/$g) / (5.0)];	# mass at each node on Floor 3
	set NodalMass5 [expr ($Floor5Weight/$g) / (5.0)];	# mass at each node on Floor 3
	set Negligible 1e-9;	# a very smnumber to avoid problems with zero

# define nodes and assign masses to beam-column intersections of frame
	# command:  node nodeID xcoord ycoord -mass mass_dof1 mass_dof2 mass_dof3
	# nodeID convention:  "xy" where x = Pier # and y = Floor # 
	node 11 $Pier1 $Floor1;
	node 21 $Pier2 $Floor1;
	node 31 $Pier3 $Floor1;
	node 41 $Pier4 $Floor1;
	node 51 $Pier5 $Floor1;
	node 61 $Pier6 $Floor1;
	node 12 $Pier1 $Floor2 -mass $NodalMass2 $Negligible $Negligible;
	node 22 $Pier2 $Floor2 -mass $NodalMass2 $Negligible $Negligible;
	node 32 $Pier3 $Floor2 -mass $NodalMass2 $Negligible $Negligible;
	node 42 $Pier4 $Floor2 -mass $NodalMass2 $Negligible $Negligible;
	node 52 $Pier5 $Floor2 -mass $NodalMass2 $Negligible $Negligible;
	node 62 $Pier6 $Floor2;
	node 13 $Pier1 $Floor3 -mass $NodalMass3 $Negligible $Negligible;
	node 23 $Pier2 $Floor3 -mass $NodalMass3 $Negligible $Negligible;
	node 33 $Pier3 $Floor3 -mass $NodalMass3 $Negligible $Negligible;
	node 43 $Pier4 $Floor3 -mass $NodalMass3 $Negligible $Negligible;
	node 53 $Pier5 $Floor3 -mass $NodalMass3 $Negligible $Negligible;
	node 63 $Pier6 $Floor3;
	node 14 $Pier1 $Floor4 -mass $NodalMass4 $Negligible $Negligible;
	node 24 $Pier2 $Floor4 -mass $NodalMass4 $Negligible $Negligible;
	node 34 $Pier3 $Floor4 -mass $NodalMass4 $Negligible $Negligible;
	node 44 $Pier4 $Floor4 -mass $NodalMass4 $Negligible $Negligible;
	node 54 $Pier5 $Floor4 -mass $NodalMass4 $Negligible $Negligible;
	node 64 $Pier6 $Floor4;
	node 15 $Pier1 $Floor5 -mass $NodalMass5 $Negligible $Negligible;
	node 25 $Pier2 $Floor5 -mass $NodalMass5 $Negligible $Negligible;
	node 35 $Pier3 $Floor5 -mass $NodalMass5 $Negligible $Negligible;
	node 45 $Pier4 $Floor5 -mass $NodalMass5 $Negligible $Negligible;
	node 55 $Pier5 $Floor5 -mass $NodalMass5 $Negligible $Negligible;
	node 65 $Pier6 $Floor5;

# define extra nodes for plastic hinge rotational springs
	# nodeID convention:  "xya" where x = Pier #, y = Floor #, a = location relative to beam-column joint
	# "a" convention: 2 = left; 3 = right;
	# "a" convention: 6 = below; 7 = above; 
	# column hinges at bottom of Story 1 (base)
	node 117 $Pier1 $Floor1;
	node 217 $Pier2 $Floor1;
	node 317 $Pier3 $Floor1;
	node 417 $Pier4 $Floor1;
	node 517 $Pier5 $Floor1;
	# column hinges at top of Story 1
	node 126 $Pier1 $Floor2;
	node 226 $Pier2 $Floor2;
	node 326 $Pier3 $Floor2;
	node 426 $Pier4 $Floor2;
	node 526 $Pier5 $Floor2;
	node 626 $Pier6 $Floor2;	# zero-stiffness spring will be used on p-delta column
	# column hinges at bottom of Story 2
	node 127 $Pier1 $Floor2;
	node 227 $Pier2 $Floor2;
	node 327 $Pier3 $Floor2;
	node 427 $Pier4 $Floor2;
	node 527 $Pier5 $Floor2;
	node 627 $Pier6 $Floor2;	# zero-stiffness spring will be used on p-delta column
	# column hinges at top of Story 2
	node 136 $Pier1 $Floor3;
	node 236 $Pier2 $Floor3;
	node 336 $Pier3 $Floor3;
	node 436 $Pier4 $Floor3;
	node 536 $Pier5 $Floor3;
	node 636 $Pier6 $Floor3;	# zero-stiffness spring will be used on p-delta column
	# column hinges at bottom of Story 3
	node 137 $Pier1 $Floor3;
	node 237 $Pier2 $Floor3;
	node 337 $Pier3 $Floor3;
	node 437 $Pier4 $Floor3;
	node 537 $Pier5 $Floor3;
	node 637 $Pier6 $Floor3;	# zero-stiffness spring will be used on p-delta column
	# column hinges at top of Story 3
	node 146 $Pier1 $Floor4;
	node 246 $Pier2 $Floor4;
	node 346 $Pier3 $Floor4;
	node 446 $Pier4 $Floor4;
	node 546 $Pier5 $Floor4;
	node 646 $Pier6 $Floor4;	# zero-stiffness spring will be used on p-delta column
	# column hinges at bottom of Story 4
	node 147 $Pier1 $Floor4;
	node 247 $Pier2 $Floor4;
	node 347 $Pier3 $Floor4;
	node 447 $Pier4 $Floor4;
	node 547 $Pier5 $Floor4;
	node 647 $Pier6 $Floor4;	# zero-stiffness spring will be used on p-delta column
	# column hinges at top of Story 4
	node 156 $Pier1 $Floor5;
	node 256 $Pier2 $Floor5;
	node 356 $Pier3 $Floor5;
	node 456 $Pier4 $Floor5;
	node 556 $Pier5 $Floor5;
	node 656 $Pier6 $Floor5;	# zero-stiffness spring will be used on p-delta column

	# beam hinges at Floor 2
	node 122 [expr $Pier1 + $phlat23] $Floor2;
	node 223 [expr $Pier2 - $phlat23] $Floor2;
	node 322 [expr $Pier2 + $phlat23] $Floor2;
	node 423 [expr $Pier3 - $phlat23] $Floor2;
	node 522 [expr $Pier3 + $phlat23] $Floor2;
	node 623 [expr $Pier4 - $phlat23] $Floor2;
	node 722 [expr $Pier4 + $phlat23] $Floor2;
	node 823 [expr $Pier5 - $phlat23] $Floor2;
	# beam hinges at Floor 3
	node 132 [expr $Pier1 + $phlat23] $Floor3;
	node 233 [expr $Pier2 - $phlat23] $Floor3;
	node 332 [expr $Pier2 + $phlat23] $Floor3;
	node 433 [expr $Pier3 - $phlat23] $Floor3;
	node 532 [expr $Pier3 + $phlat23] $Floor3;
	node 633 [expr $Pier4 - $phlat23] $Floor3;
	node 732 [expr $Pier4 + $phlat23] $Floor3;
	node 833 [expr $Pier5 - $phlat23] $Floor3;
	# beam hinges at Floor 4
	node 142 [expr $Pier1 + $phlat23] $Floor4;
	node 243 [expr $Pier2 - $phlat23] $Floor4;
	node 342 [expr $Pier2 + $phlat23] $Floor4;
	node 443 [expr $Pier3 - $phlat23] $Floor4;
	node 542 [expr $Pier3 + $phlat23] $Floor4;
	node 643 [expr $Pier4 - $phlat23] $Floor4;
	node 742 [expr $Pier4 + $phlat23] $Floor4;
	node 843 [expr $Pier5 - $phlat23] $Floor4;
	# beam hinges at Floor 5
	node 152 [expr $Pier1 + $phlat23] $Floor5;
	node 253 [expr $Pier2 - $phlat23] $Floor5;
	node 352 [expr $Pier2 + $phlat23] $Floor5;
	node 453 [expr $Pier3 - $phlat23] $Floor5;
	node 552 [expr $Pier3 + $phlat23] $Floor5;
	node 653 [expr $Pier4 - $phlat23] $Floor5;
	node 752 [expr $Pier4 + $phlat23] $Floor5;
	node 853 [expr $Pier5 - $phlat23] $Floor5;

# constrain beam-column joints in a floor to have the same lateral displacement using the "equalDOF" command
	# command: equalDOF $MasterNodeID $SlaveNodeID $dof1 $dof2...
	set dof1 1;	# constrain movement in dof 1 (x-direction)
	equalDOF 12 22 $dof1;	# Floor 2:  Pier 1 to Pier 2
	equalDOF 12 32 $dof1;	# Floor 2:  Pier 1 to Pier 3
	equalDOF 12 42 $dof1;	# Floor 2:  Pier 1 to Pier 4
	equalDOF 12 52 $dof1;	# Floor 2:  Pier 1 to Pier 5
	equalDOF 13 23 $dof1;	# Floor 3:  Pier 1 to Pier 2
	equalDOF 13 33 $dof1;	# Floor 3:  Pier 1 to Pier 3
	equalDOF 13 43 $dof1;	# Floor 3:  Pier 1 to Pier 4
	equalDOF 13 53 $dof1;	# Floor 3:  Pier 1 to Pier 5
	equalDOF 14 24 $dof1;	# Floor 4:  Pier 1 to Pier 2
	equalDOF 14 34 $dof1;	# Floor 4:  Pier 1 to Pier 3
	equalDOF 14 44 $dof1;	# Floor 4:  Pier 1 to Pier 4
	equalDOF 14 54 $dof1;	# Floor 5:  Pier 1 to Pier 5
	equalDOF 15 25 $dof1;	# Floor 5:  Pier 1 to Pier 2
	equalDOF 15 35 $dof1;	# Floor 5:  Pier 1 to Pier 3
	equalDOF 15 45 $dof1;	# Floor 5:  Pier 1 to Pier 4
	equalDOF 15 55 $dof1;	# Floor 5:  Pier 1 to Pier 5

# assign boundary condidtions 
	# command:  fix nodeID dxFixity dyFixity rzFixity
	# fixity values: 1 = constrained; 0 = unconstrained
	# fix the base of the building; pin P-delta column at base
	fix 11 1 1 1;
	fix 21 1 1 1;
	fix 31 1 1 1;
	fix 41 1 1 1;
	fix 51 1 1 1;
	fix 61 1 1 0;	# P-delta column is pinned

###################################################################################################
#          Define Section Properties and Elements													  
###################################################################################################
# define material properties
	set Es 29000.0;			# steel Young's modulus

# define column section W24x207 for Story 1 & 2
	set Acol_12  60.7;		# cross-sectional area
	set Icol_12  6820.0;	# moment of inertia
	set Mycol_12 33330.0;	# yield moment

# define column section W24x162 for Story 3 & 4
	set Acol_34  47.7;		# cross-sectional area
	set Icol_34  5170.0;	# moment of inertia
	set Mycol_34 25740.0;	# yield moment

# define beam section W30x108 for Floor 2 & 3
	set Abeam_23  31.7;		# cross-sectional area (full section properties)
	set Ibeam_23  4470.0;	# moment of inertia  (full section properties)
	set Mybeam_23 13293.5;	# yield moment at plastic hinge location (i.e., My of RBS section, if used)
	# note: In this example the hinges form right at the beam-column joint, so using an RBS doesn't make sense; 
	#		however, it is done here simply for illustrative purposes.

# define beam section W24x84 for Floor 4 & 5
	set Abeam_45  24.7;		# cross-sectional area (full section properties)
	set Ibeam_45  2370.0;	# moment of inertia  (full section properties)
	set Mybeam_45 8310.5;	# yield moment at plastic hinge location (i.e., My of RBS section, if used)
	# note: In this example the hinges form right at the beam-column joint, so using an RBS doesn't make sense; 
	#		however, it is done here simply for illustrative purposes.
	
# determine stiffness modifications to equate the stiffness of the spring-elastic element-spring subassembly to the stiffness of the actual frame member
	# References: (1) Ibarra, L. F., and Krawinkler, H. (2005). "Global collapse of frame structures under seismic excitations," Technical Report 152,
	#             		The John A. Blume Earthquake Engineering Research Center, Department of Civil Engineering, Stanford University, Stanford, CA.
	#			  (2) Zareian, F. and Medina, R. A. (2010). �A practical method for proper modeling of structural damping in inelastic plane
	#					structural systems,� Computers & Structures, Vol. 88, 1-2, pp. 45-53.
	# calculate modified section properties to account for spring stiffness being in series with the elastic element stiffness
	set n 10.0;		# stiffness multiplier for rotational spring

	# calculate modified moment of inertia for elastic elements
	set Icol_12mod  [expr $Icol_12*($n+1.0)/$n];	# modified moment of inertia for columns in Story 1 & 2
	set Icol_34mod  [expr $Icol_34*($n+1.0)/$n];	# modified moment of inertia for columns in Story 3 & 4
	set Ibeam_23mod [expr $Ibeam_23*($n+1.0)/$n];	# modified moment of inertia for beams in Floor 2 & 3
	set Ibeam_45mod [expr $Ibeam_45*($n+1.0)/$n];	# modified moment of inertia for beams in Floor 4 & 5
	
	# calculate modified rotational stiffness for plastic hinge springs
	set Ks_col_1   [expr $n*6.0*$Es*$Icol_12mod/$HStory1];		# rotational stiffness of Story 1 column springs
	set Ks_col_2   [expr $n*6.0*$Es*$Icol_12mod/$HStoryTyp];	# rotational stiffness of Story 2 column springs
	set Ks_col_3   [expr $n*6.0*$Es*$Icol_34mod/$HStoryTyp];	# rotational stiffness of Story 3 column springs
	set Ks_col_4   [expr $n*6.0*$Es*$Icol_34mod/$HStoryTyp];	# rotational stiffness of Story 4 column springs
	set Ks_beam_23 [expr $n*6.0*$Es*$Ibeam_23mod/$WBay];		# rotational stiffness of Floor 2 & 3 beam springs
	set Ks_beam_45 [expr $n*6.0*$Es*$Ibeam_45mod/$WBay];		# rotational stiffness of Floor 4 & 5 beam springs
	
# set up geometric transformations of element
	set PDeltaTransf 1;
	geomTransf PDelta $PDeltaTransf; 	# PDelta transformation

# define elastic column elements using "element" command
	# command: element elasticBeamColumn $eleID $iNode $jNode $A $E $I $transfID
	# eleID convention:  "1xy" where 1 = col, x = Pier #, y = Story #
	# Columns Story 1
	element elasticBeamColumn  111  117 126 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 1
	element elasticBeamColumn  121  217 226 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 2
	element elasticBeamColumn  131  317 326 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 3
	element elasticBeamColumn  141  417 426 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 4
	element elasticBeamColumn  151  517 526 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 5
	# Columns Story 2
	element elasticBeamColumn  112  127 136 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 1
	element elasticBeamColumn  122  227 236 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 2
	element elasticBeamColumn  132  327 336 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 3
	element elasticBeamColumn  142  427 436 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 4
	element elasticBeamColumn  152  527 536 $Acol_12 $Es $Icol_12mod $PDeltaTransf;	# Pier 5
	# Columns Story 3
	element elasticBeamColumn  113  137 146 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 1
	element elasticBeamColumn  123  237 246 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 2
	element elasticBeamColumn  133  337 346 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 3
	element elasticBeamColumn  143  437 446 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 4
	element elasticBeamColumn  153  537 546 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 5
	# Columns Story 4
	element elasticBeamColumn  114  147 156 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 1
	element elasticBeamColumn  124  247 256 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 2
	element elasticBeamColumn  134  347 356 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 3
	element elasticBeamColumn  144  447 456 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 4
	element elasticBeamColumn  154  547 556 $Acol_34 $Es $Icol_34mod $PDeltaTransf;	# Pier 5
	
# define elastic beam elements
	# eleID convention:  "2xy" where 2 = beam, x = Bay #, y = Floor #
	# Beams Story 1
	element elasticBeamColumn  212  122 223 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	element elasticBeamColumn  213  322 423 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	element elasticBeamColumn  214  522 623 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	element elasticBeamColumn  215  722 823 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	# Beams Story 2
	element elasticBeamColumn  222  132 233 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	element elasticBeamColumn  223  332 433 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	element elasticBeamColumn  224  532 633 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	element elasticBeamColumn  225  732 833 $Abeam_23 $Es $Ibeam_23mod $PDeltaTransf;
	# Beams Story 3
	element elasticBeamColumn  232  142 243 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	element elasticBeamColumn  233  342 443 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	element elasticBeamColumn  234  542 643 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	element elasticBeamColumn  235  742 843 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	# Beams Story 4
	element elasticBeamColumn  242  152 253 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	element elasticBeamColumn  243  352 453 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	element elasticBeamColumn  244  552 653 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	element elasticBeamColumn  245  752 853 $Abeam_45 $Es $Ibeam_45mod $PDeltaTransf;
	
# define p-delta columns and rigid links
	set TrussMatID 600;		# define a material ID
	set Arigid 1000.0;		# define area of truss section (make much larger than A of frame elements)
	set Irigid 100000.0;	# moment of inertia for p-delta columns  (make much larger than I of frame elements)
	uniaxialMaterial Elastic $TrussMatID $Es;		# define truss material
	# rigid links
	# command: element truss $eleID $iNode $jNode $A $materialID
	# eleID convention:  6xy, 6 = truss link, x = Bay #, y = Floor #
	element truss  622 52 62 $Arigid $TrussMatID;	# Floor 2
	element truss  623 53 63 $Arigid $TrussMatID;	# Floor 3
	element truss  624 54 64 $Arigid $TrussMatID;	# Floor 4
	element truss  625 55 65 $Arigid $TrussMatID;	# Floor 5
	
	# p-delta columns
	# eleID convention:  7xy, 7 = p-delta columns, x = Pier #, y = Story #
	element elasticBeamColumn  731  61  626 $Arigid $Es $Irigid $PDeltaTransf;	# Story 1
	element elasticBeamColumn  732  627 636 $Arigid $Es $Irigid $PDeltaTransf;	# Story 2
	element elasticBeamColumn  734  637 646 $Arigid $Es $Irigid $PDeltaTransf;	# Story 3
	element elasticBeamColumn  735  647 656 $Arigid $Es $Irigid $PDeltaTransf;	# Story 4
	
###################################################################################################
#          Define Rotational Springs for Plastic Hinges												  
###################################################################################################
# define rotational spring properties and create spring elements using "rotSpring2DModIKModel" procedure
	# rotSpring2DModIKModel creates a uniaxial material spring with a bilinear response based on Modified Ibarra Krawinkler Deterioration Model
	# references provided in rotSpring2DModIKModel.tcl
	# input values for Story 1 column springs
	set McMy 1.05;			# ratio of capping moment to yield moment, Mc / My
	set LS 1000.0;			# basic strength deterioration (a very large # = no cyclic deterioration)
	set LK 1000.0;			# unloading stiffness deterioration (a very large # = no cyclic deterioration)
	set LA 1000.0;			# accelerated reloading stiffness deterioration (a very large # = no cyclic deterioration)
	set LD 1000.0;			# post-capping strength deterioration (a very large # = no deterioration)
	set cS 1.0;				# exponent for basic strength deterioration (c = 1.0 for no deterioration)
	set cK 1.0;				# exponent for unloading stiffness deterioration (c = 1.0 for no deterioration)
	set cA 1.0;				# exponent for accelerated reloading stiffness deterioration (c = 1.0 for no deterioration)
	set cD 1.0;				# exponent for post-capping strength deterioration (c = 1.0 for no deterioration)
	set th_pP 0.028;		# plastic rot capacity for pos loading
	set th_pN 0.028;		# plastic rot capacity for neg loading
	set th_pcP 0.331;			# post-capping rot capacity for pos loading
	set th_pcN 0.331;			# post-capping rot capacity for neg loading
	set ResP 0.4;			# residual strength ratio for pos loading
	set ResN 0.4;			# residual strength ratio for neg loading
	set th_uP 0.4;			# ultimate rot capacity for pos loading
	set th_uN 0.4;			# ultimate rot capacity for neg loading
	set DP 1.0;				# rate of cyclic deterioration for pos loading
	set DN 1.0;				# rate of cyclic deterioration for neg loading
	set a_mem [expr ($n+1.0)*($Mycol_12*($McMy-1.0)) / ($Ks_col_1*$th_pP)];	# strain hardening ratio of spring
	set b [expr ($a_mem)/(1.0+$n*(1.0-$a_mem))];							# modified strain hardening ratio of spring (Ibarra & Krawinkler 2005, note: Eqn B.5 is incorrect)

# define column springs
	# Spring ID: "3xya", where 3 = col spring, x = Pier #, y = Story #, a = location in story
	# "a" convention: 1 = bottom of story, 2 = top of story
	# command: rotSpring2DModIKModel	id    ndR  ndC     K   asPos  asNeg  MyPos      MyNeg      LS    LK    LA    LD   cS   cK   cA   cD  th_p+   th_p-   th_pc+   th_pc-  Res+   Res-   th_u+   th_u-    D+     D-
	# col springs @ bottom of Story 1 (at base)
	rotSpring2DModIKModel 3111 11 117 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3211 21 217 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3311 31 317 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3411 41 417 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3511 51 517 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	#col springs @ top of Story 1 (below Floor 2)
	rotSpring2DModIKModel 3112 12 126 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3212 22 226 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3312 32 326 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3412 42 426 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3512 52 526 $Ks_col_1 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

	# recompute strain hardening since Story 2 is not the same height as Story 1
	set a_mem [expr ($n+1.0)*($Mycol_12*($McMy-1.0)) / ($Ks_col_2*$th_pP)];	# strain hardening ratio of spring
	set b [expr ($a_mem)/(1.0+$n*(1.0-$a_mem))];							# modified strain hardening ratio of spring (Ibarra & Krawinkler 2005, note: there is mistake in Eqn B.5)
	# col springs @ bottom of Story 2 (above Floor 2)
	rotSpring2DModIKModel 3121 12 127 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3221 22 227 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3321 32 327 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3421 42 427 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3521 52 527 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	#col springs @ top of Story 2 (below Floor 3)
	rotSpring2DModIKModel 3122 13 136 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3222 23 236 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3322 33 336 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3422 43 436 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3522 53 536 $Ks_col_2 $b $b $Mycol_12 [expr -$Mycol_12] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

	# recompute strain hardening since Story 3 is not the same height as Story 2
	set th_pP 0.026;		# plastic rot capacity for pos loading
	set th_pN 0.026;		# plastic rot capacity for neg loading
	set th_pcP 0.24;			# post-capping rot capacity for pos loading
	set th_pcN 0.24;			# post-capping rot capacity for neg loading
	set a_mem [expr ($n+1.0)*($Mycol_34*($McMy-1.0)) / ($Ks_col_3*$th_pP)];	# strain hardening ratio of spring
	set b [expr ($a_mem)/(1.0+$n*(1.0-$a_mem))];							# modified strain hardening ratio of spring (Ibarra & Krawinkler 2005, note: there is mistake in Eqn B.5)
	# col springs @ bottom of Story 3 (above Floor 3)
	rotSpring2DModIKModel 3131 13 137 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3231 23 237 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3331 33 337 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3431 43 437 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3531 53 537 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	#col springs @ top of Story 3 (below Floor 4)
	rotSpring2DModIKModel 3132 14 146 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3232 24 246 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3332 34 346 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3432 44 446 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3532 54 546 $Ks_col_3 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

	# recompute strain hardening since Story 4 is not the same height as Story 3
	set a_mem [expr ($n+1.0)*($Mycol_34*($McMy-1.0)) / ($Ks_col_4*$th_pP)];	# strain hardening ratio of spring
	set b [expr ($a_mem)/(1.0+$n*(1.0-$a_mem))];							# modified strain hardening ratio of spring (Ibarra & Krawinkler 2005, note: there is mistake in Eqn B.5)
	# col springs @ bottom of Story 4 (above Floor 4)
	rotSpring2DModIKModel 3141 14 147 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3241 24 247 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3341 34 347 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3441 44 447 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3541 54 547 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	#col springs @ top of Story 4 (below Floor 5)
	rotSpring2DModIKModel 3142 15 156 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3242 25 256 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3342 35 356 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3442 45 456 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 3542 55 556 $Ks_col_4 $b $b $Mycol_34 [expr -$Mycol_34] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	
	# create region for frame column springs
	# command: region $regionID -ele $ele_1_ID $ele_2_ID...
	region 1 -ele 3111 3211 3311 3411 3511 3112 3212 3312 3412 3512 3121 3221 3321 3421 3521 3122 3222 3322 3422 3522 \
	              3131 3231 3331 3431 3531 3132 3232 3332 3432 3532 3141 3241 3341 3441 3541 3142 3242 3342 3442 3542;
	
# define beam springs
	# Spring ID: "4xya", where 4 = beam spring, x = Bay #, y = Floor #, a = location in bay
	# "a" convention: 1 = left end, 2 = right end
	# redefine the rotations since they are not the same
	set th_pP 0.017;
	set th_pN 0.017;
	set th_pcP 0.132;
	set th_pcN 0.132;
	set a_mem [expr ($n+1.0)*($Mybeam_23*($McMy-1.0)) / ($Ks_beam_23*$th_pP)];	# strain hardening ratio of spring
	set b [expr ($a_mem)/(1.0+$n*(1.0-$a_mem))];								# modified strain hardening ratio of spring (Ibarra & Krawinkler 2005, note: there is mistake in Eqn B.5)
	#beam springs at Floor 2
	rotSpring2DModIKModel 4121 12 122 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4122 22 223 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4221 22 322 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4222 32 423 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4321 32 522 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4322 42 623 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4421 42 722 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4422 52 823 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

	#beam springs at Floor 3
	rotSpring2DModIKModel 4131 13 132 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4132 23 233 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4231 23 332 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4232 33 433 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4331 33 532 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4332 43 633 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4431 43 732 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4432 53 833 $Ks_beam_23 $b $b $Mybeam_23 [expr -$Mybeam_23] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

	#beam springs at Floor 4
	# redefine the rotations since they are not the same
	set th_pP 0.02;
	set th_pN 0.02;
	set th_pcP 0.156;
	set th_pcN 0.156;
	set a_mem [expr ($n+1.0)*($Mybeam_45*($McMy-1.0)) / ($Ks_beam_45*$th_pP)];	# strain hardening ratio of spring
	set b [expr ($a_mem)/(1.0+$n*(1.0-$a_mem))];								# modified strain hardening ratio of spring (Ibarra & Krawinkler 2005, note: there is mistake in Eqn B.5)
	rotSpring2DModIKModel 4141 14 142 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4142 24 243 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4241 24 342 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4242 34 443 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4341 34 542 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4342 44 643 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4441 44 742 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4442 54 843 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;

	#beam springs at Floor 5
	rotSpring2DModIKModel 4151 15 152 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4152 25 253 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4251 25 352 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4252 35 453 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4351 35 552 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4352 45 653 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4451 45 752 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	rotSpring2DModIKModel 4452 55 853 $Ks_beam_45 $b $b $Mybeam_45 [expr -$Mybeam_45] $LS $LK $LA $LD $cS $cK $cA $cD $th_pP $th_pN $th_pcP $th_pcN $ResP $ResN $th_uP $th_uN $DP $DN;
	
	# create region for beam springs
	region 2 -ele 4121 4122 4221 4222 4321 4322 4421 4422 4131 4132 4231 4232 4331 4332 4431 4432 \
	              4141 4142 4241 4242 4341 4342 4441 4442 4151 4152 4251 4252 4352 4352 4451 4452;
	
# define p-delta column spring: zero-stiffness elastic spring	
	#Spring ID: "5xya" where 5 = leaning column spring, x = Pier #, y = Story #, a = location in story
	# "a" convention: 1 = bottom of story, 2 = top of story
	# rotLeaningCol ElemID ndR ndC 
	rotLeaningCol 5312 62 626;	# top of Story 1
	rotLeaningCol 5321 62 627;	# bottom of Story 2
	rotLeaningCol 5322 63 636;	# top of Story 2
	rotLeaningCol 5331 63 637;	# bottom of Story 3
	rotLeaningCol 5332 64 646;	# top of Story 3
	rotLeaningCol 5341 64 647;	# bottom of Story 4
	rotLeaningCol 5342 65 656;	# top of Story 4
	
	# create region for P-Delta column springs
	region 3 -ele 5312 5321 5322 5331 5332 5341 5342;
	
############################################################################
#              Gravity Loads & Gravity Analysis
############################################################################
# apply gravity loads
	#command: pattern PatternType $PatternID TimeSeriesType
	pattern Plain 11 Constant {
		
		# point loads on leaning column nodes
		# command: load node Fx Fy Mz
		set P_PD2 [expr -964.0];	# Floor 2
		set P_PD3 [expr -964.0];	# Floor 3
		set P_PD4 [expr -964.0];	# Floor 4
		set P_PD5 [expr -964.0];	# Floor 5
		load 62 0.0 $P_PD2 0.0;		# Floor 2
		load 63 0.0 $P_PD3 0.0;		# Floor 3
		load 64 0.0 $P_PD4 0.0;		# Floor 4
		load 65 0.0 $P_PD5 0.0;		# Floor 5
		
		# point loads on frame column nodes
		set P_F2 [expr 0.2*(-1.0*$Floor2Weight-$P_PD2)];	# load on each frame node in Floor 2
		set P_F3 [expr 0.2*(-1.0*$Floor3Weight-$P_PD3)];	# load on each frame node in Floor 3
		set P_F4 [expr 0.2*(-1.0*$Floor4Weight-$P_PD4)];	# load on each frame node in Floor 4
		set P_F5 [expr 0.2*(-1.0*$Floor5Weight-$P_PD5)];	# load on each frame node in Floor 5
		# Floor 2 loads
		load 12 0.0 $P_F2 0.0;
		load 22 0.0 $P_F2 0.0;
		load 32 0.0 $P_F2 0.0;
		load 42 0.0 $P_F2 0.0;
		load 52 0.0 $P_F2 0.0;		
		# Floor 3 loads		
		load 13 0.0 $P_F3 0.0;
		load 23 0.0 $P_F3 0.0;
		load 33 0.0 $P_F3 0.0;
		load 43 0.0 $P_F3 0.0;
		load 53 0.0 $P_F3 0.0;
		# Floor 4 loads		
		load 14 0.0 $P_F4 0.0;
		load 24 0.0 $P_F4 0.0;
		load 34 0.0 $P_F4 0.0;
		load 44 0.0 $P_F4 0.0;
		load 54 0.0 $P_F4 0.0;
		# Floor 5 loads		
		load 15 0.0 $P_F5 0.0;
		load 25 0.0 $P_F5 0.0;
		load 35 0.0 $P_F5 0.0;
		load 45 0.0 $P_F5 0.0;
		load 55 0.0 $P_F5 0.0;
	}

# Gravity-analysis: load-controlled static analysis
	set Tol 1.0e-6;							# convergence tolerance for test
	constraints Plain;						# how it handles boundary conditions
	numberer RCM;							# renumber dof's to minimize band-width (optimization)
	system BandGeneral;						# how to store and solve the system of equations in the analysis (large model: try UmfPack)
	test NormDispIncr $Tol 6;				# determine if convergence has been achieved at the end of an iteration step
	algorithm Newton;						# use Newton's solution algorithm: updates tangent stiffness at every iteration
	set NstepGravity 10;					# apply gravity in 10 steps
	set DGravity [expr 1.0/$NstepGravity];	# load increment
	integrator LoadControl $DGravity;		# determine the next time step for an analysis
	analysis Static;						# define type of analysis: static or transient
	analyze $NstepGravity;					# apply gravity


############################################################################
#                       Eigenvalue Analysis                    			   
############################################################################
	set pi [expr 2.0*asin(1.0)];						# Definition of pi
	set nEigenI 1;										# mode i = 1
	set nEigenJ 2;										# mode j = 2
	set lambdaN [eigen [expr $nEigenJ]];				# eigenvalue analysis for nEigenJ modes
	set lambdaI [lindex $lambdaN [expr 0]];				# eigenvalue mode i = 1
	set lambdaJ [lindex $lambdaN [expr $nEigenJ-1]];	# eigenvalue mode j = 2
	set w1 [expr pow($lambdaI,0.5)];					# w1 (1st mode circular frequency)
	set w2 [expr pow($lambdaJ,0.5)];					# w2 (2nd mode circular frequency)
	set T1 [expr 2.0*$pi/$w1];							# 1st mode period of the structure
	set T2 [expr 2.0*$pi/$w2];							# 2nd mode period of the structure
	puts "T1 = $T1 s";									# display the first mode period in the command window
	puts "T2 = $T2 s";									# display the second mode period in the command window

	# maintain constant gravity loads and reset time to zero
	loadConst -time 0.0
	puts "Model Built"

