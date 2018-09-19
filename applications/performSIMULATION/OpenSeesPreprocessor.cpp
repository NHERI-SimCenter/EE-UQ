#include "OpenSeesPreprocessor.h"
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <map>
#include <iostream>

OpenSeesPreprocessor::OpenSeesPreprocessor()
  :rootBIM(0), rootSAM(0), rootEVENT(0), rootEDP(0), rootSIM(0), 
   fileBIM(0), fileSAM(0), fileEVENT(0), fileEDP(0), fileSIM(0),
   analysisType(-1), numSteps(0), dT(0.0), nStory(0), mapping(0)
{

}

OpenSeesPreprocessor::~OpenSeesPreprocessor() {

}

int 
OpenSeesPreprocessor::writeRV(const char *BIM,
			      const char *SAM,
			      const char *EVENT,
			      const char *SIM)
{
  // json object to fill in with data for simulation
  rootSIM = json_object();

  //
  // open input file & get Simulation data
  //

  json_error_t error;
  rootBIM = json_load_file(BIM, 0, &error);
  json_t *sim = json_object_get(rootBIM,"Simulation");

  if (sim == NULL) {
    json_dump_file(rootSIM,SIM,0);
    fprintf(stderr, "OpenSeesPreprocessor - writeRV Simulation data not found\n");
    return -1;
  } 

  //
  // TO DO .. check simulation data exists and contains all fields
  //  .. would stop dakota from runnning
  //

  //
  // get the Simulation data & write to SIM file
  //  duplicating data with intent not to open 2 files later
  //



  json_dump_file(sim, SIM,0);

  return 0;
}

int 
OpenSeesPreprocessor::createInputFile(const char *BIM,
				      const char *SAM,
				      const char *EVENT,
				      const char *EDP,
				      const char *SIM,
				      const char *filenameTCL)
{

  fileBIM = BIM;

  //
  // open tcl script
  // 

  ofstream *s = new ofstream;
  s->open(filenameTCL, ios::out);
  ofstream &tclFile = *s;

  // 
  // load the SAM file
  //

  json_error_t error;
  rootSAM = json_load_file(SAM, 0, &error);

  bool processedSAM = false;

  // if non-standard type of SAM need to special process
  json_t *type = json_object_get(rootSAM,"type");

  if (type != NULL) {

    const char *typeSAM = json_string_value(type);
    
    if (strcmp(typeSAM,"OpenSeesInput") == 0) {
      
      json_t *theRVs = json_object_get(rootSAM, "randomVar");
      json_t *theRV;
      int index = 0;
      tclFile << "# Random Variables:\n";
      json_array_foreach(theRVs, index, theRV) {
	json_t *rvName = json_object_get(theRV,"name");
	json_t *rvValue = json_object_get(theRV,"value");
	const char *name = json_string_value(rvName);      
	double value = json_real_value(rvValue);      
	
	tclFile << "pset " << name << " " << value << "\n";
      }
      
      json_t *script = json_object_get(rootSAM,"mainScript");
      const char *scriptName = json_string_value(script);
      tclFile << "\n# Main Script: \nsource " << scriptName << "\n";

      json_t *ndm = json_object_get(rootSAM,"ndm");
      NDM = json_integer_value(ndm);
      if (NDM == 1) NDF = 1;
      if (NDM == 2) NDF = 2;
      if (NDM == 3) NDF = 3;
      printf("NDM: %d\n",NDM);
      processedSAM = true;
    }
  }

  mapping = json_object_get(rootSAM,"NodeMapping");  
  if (mapping == NULL) {
    std::cerr << "OpenSeesPreprocessor - no NodeMapping in SAM file\n";
    return -1;
  }

  if (processedSAM == false) {
    // a regular SAM file, create node elements and materials as normal
    processNodes(tclFile);
    processMaterials(tclFile);
    processElements(tclFile);
  }

  // process damping
  rootSIM = json_load_file(SIM, 0, &error);
  processDamping(tclFile);

  //
  // process events
  //   - creates events and does analysis
  //

  rootEVENT = json_load_file(EVENT, 0, &error);
  rootEDP = json_load_file(EDP, 0, &error);

  processEvents(tclFile);

  // close file
  s->close();
  return 0;
}



int 
OpenSeesPreprocessor::processMaterials(ofstream &s){
  int index;
  json_t *material;

  json_t *propertiesObject = json_object_get(rootSAM,"Properties");  
  json_t *materials = json_object_get(propertiesObject,"uniaxialMaterials");
  json_array_foreach(materials, index, material) {
    const char *type = json_string_value(json_object_get(material,"type"));
    
    if (strcmp(type,"shear") == 0) {
      int tag = json_integer_value(json_object_get(material,"name"));
      double K0 = json_real_value(json_object_get(material,"K0"));
      double Sy = json_real_value(json_object_get(material,"Sy"));
      double eta = json_real_value(json_object_get(material,"eta"));
      double C = json_real_value(json_object_get(material,"C"));
      double gamma = json_real_value(json_object_get(material,"gamma"));
      double alpha = json_real_value(json_object_get(material,"alpha"));
      double beta = json_real_value(json_object_get(material,"beta"));
      double omega = json_real_value(json_object_get(material,"omega"));
      double eta_soft = json_real_value(json_object_get(material,"eta_soft"));
      double a_k = json_real_value(json_object_get(material,"a_k"));
      //s << "uniaxialMaterial Elastic " << tag << " " << K0 << "\n";
      if (K0==0)
          K0=1.0e-6;
      if (eta==0)
          eta=1.0e-6;
      //uniaxialMaterial Hysteretic $matTag $s1p $e1p $s2p $e2p <$s3p $e3p>
      //$s1n $e1n $s2n $e2n <$s3n $e3n> $pinchX $pinchY $damage1 $damage2 <$beta>
      double e2p = Sy/K0+(alpha-1)*Sy/eta/K0;
      double e3p = 1.0;
      if(e2p >= 1.0)
        e3p = 10.0 * e2p;

      double e2n = -(beta*Sy/K0 + beta*(alpha-1)*Sy/eta/K0);
      double e3n = -1.0;
      if(e2n <= -1.0)
        e3n = 10.0 * e2n;

      s << "uniaxialMaterial Hysteretic " << tag << " " << Sy << " " << Sy/K0
        << " " << alpha*Sy << " " << e2p
        << " " << alpha*Sy << " " << e3p
        << " " << -beta*Sy << " " << -beta*Sy/K0 << " " << -beta*(alpha*Sy)
        << " " << e2n
        << " " << -beta*(alpha*Sy) << " " << e3n
        << " " << gamma
        << " " << gamma << " " << 0.0 << " " << 0.0 << " " << a_k << "\n";
    }
  }
  return 0;
}

int 
OpenSeesPreprocessor::processSections(ofstream &s) {
  return 0;
}

int 
OpenSeesPreprocessor::processNodes(ofstream &s){
  int index;
  json_t *node;

  json_t *geometry = json_object_get(rootSAM,"Geometry");  
  json_t *nodes = json_object_get(geometry,"nodes");

  NDM = 0;
  NDF = 0;

  json_array_foreach(nodes, index, node) {

    int tag = json_integer_value(json_object_get(node,"name"));
    if(nStory<tag)  nStory=tag;
    json_t *crds = json_object_get(node,"crd");
    int sizeCRD = json_array_size(crds);
    int ndf = json_integer_value(json_object_get(node,"ndf"));

    if (sizeCRD != NDM || ndf != NDF) {
      NDM = sizeCRD;
      NDF = ndf;
      // issue new model command if node size changes
      s << "model BasicBuilder -ndm " << NDM << " -ndf " << NDF << "\n";
    } 

    s << "node " << tag << " ";
    json_t *crd;
    int crdIndex;
    json_array_foreach(crds, crdIndex, crd) {
      s << json_real_value(crd) << " " ;
    }

    json_t *mass = json_object_get(node,"mass");
    if (mass != NULL) {
      s << "-mass ";
      double massV = json_real_value(mass);
      for (int i=0; i<NDF; i++)
	s << massV << " " ;
    }

    s << "\n";
  }

  int nodeTag = getNode("1","1");
  s << "fix " << nodeTag;
  for (int i=0; i<NDF; i++)
     s << " " << 1;
  s << "\n";

  return 0;
}

int 
OpenSeesPreprocessor::processElements(ofstream &s){
  int index;
  json_t *element;

  json_t *geometry = json_object_get(rootSAM,"Geometry");  
  json_t *elements = json_object_get(geometry,"elements");

  json_array_foreach(elements, index, element) {

    int tag = json_integer_value(json_object_get(element,"name"));
    const char *type = json_string_value(json_object_get(element,"type"));
    if (strcmp(type,"shear_beam") == 0) {
      s << "element zeroLength " << tag << " " ;
      json_t *nodes = json_object_get(element,"nodes");
      json_t *nodeTag;
      int nodeIndex;
      json_array_foreach(nodes, nodeIndex, nodeTag) {
	s << json_integer_value(nodeTag) << " " ;
      }

      int matTag = json_integer_value(json_object_get(element,"uniaxial_material"));
      s << "-mat " << matTag << " -dir 1 \n";
    }
    else if (strcmp(type,"shear_beam2d") == 0) {
      s << "element zeroLength " << tag << " " ;
      json_t *nodes = json_object_get(element,"nodes");
      json_t *nodeTag;
      int nodeIndex;
      json_array_foreach(nodes, nodeIndex, nodeTag) {
	s << json_integer_value(nodeTag) << " " ;
      }

      int matTag = json_integer_value(json_object_get(element,"uniaxial_material"));
      s << "-mat " << matTag << " " << matTag << " -dir 1 2\n";
    }
  }
  return 0;
}


int
OpenSeesPreprocessor::processDamping(ofstream &s){
    double damping = json_real_value(json_object_get(rootSIM,"dampingRatio"));
    s << "set xDamp " << damping << ";\n"
      << "set MpropSwitch 1.0;\n"
      << "set KcurrSwitch 0.0;\n"
      << "set KinitSwitch 0.0;\n"
      << "set KcommSwitch 1.0;\n"
      << "set nEigenI 1;\n";

    json_t *geometry = json_object_get(rootSAM,"Geometry");
    json_t *nodes = json_object_get(geometry,"nodes");
    int nStory = json_array_size(nodes)-1;
    int nEigenJ=0;
    if (nStory <= 0) {
      nEigenJ = 2;
      nStory = 1;
    } else if (nStory<=2)
      nEigenJ=nStory*2;   //first mode or second mode
    else
      nEigenJ=3*2;          

     s << "set nEigenJ "<<nEigenJ<<";\n"
       << "set lambdaN [eigen -fullGenLapack "<< nEigenJ <<"];\n"
       << "set lambdaI [lindex $lambdaN [expr $nEigenI-1]];\n"
       << "set lambdaJ [lindex $lambdaN [expr $nEigenJ-1]];\n"
       << "set omegaI [expr pow($lambdaI,0.5)];\n"
       << "set omegaJ [expr pow($lambdaJ,0.5)];\n"
       << "set alphaM [expr $MpropSwitch*$xDamp*(2*$omegaI*$omegaJ)/($omegaI+$omegaJ)];\n"
       << "set betaKcurr [expr $KcurrSwitch*2.*$xDamp/($omegaI+$omegaJ)];\n"
       << "set betaKinit [expr $KinitSwitch*2.*$xDamp/($omegaI+$omegaJ)];\n"
       << "set betaKcomm [expr $KcommSwitch*2.*$xDamp/($omegaI+$omegaJ)];\n"
       << "rayleigh $alphaM $betaKcurr $betaKinit $betaKcomm;\n";

     return 0;
}


int 
OpenSeesPreprocessor::processEvents(ofstream &s){

  //
  // foreach EVENT
  //   create load pattern
  //   add recorders
  //   add analysis script
  //

  int numTimeSeries = 101;
  int numPatterns = 101;

  int index;
  json_t *event;

  json_t *events = json_object_get(rootEVENT,"Events");  
  json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  

  int numEvents = json_array_size(events);
  int numEDPs = json_array_size(edps);

  s << "loadConst -time 0.0\n";

  for (int i=0; i<numEvents; i++) {
    
    //
    // process an event
    //

    json_t *event = json_array_get(events,i);
    processEvent(s,event,numPatterns,numTimeSeries);
    const char *eventName = json_string_value(json_object_get(event,"name"));

    // create recorder foreach EDP
    // loop through EDPs and find corresponding EDP

    char edpEventName[50];
    
    for (int j=0; j<numEDPs; j++) {
      printf("EDP: %d\n",j);

      json_t *eventEDPs = json_array_get(edps, j);
      //      const char *edpEventName = json_string_value(json_object_get(eventEDPs,"name"));
      //      if (strcmp(edpEventName, eventName) == 0) {
      sprintf(edpEventName,"%d",j);

	json_t *eventEDP = json_object_get(eventEDPs,"responses");
	int numResponses = json_array_size(eventEDP);

	for (int k=0; k<numResponses; k++) {


	  json_t *response = json_array_get(eventEDP, k);
	  const char *type = json_string_value(json_object_get(response, "type"));

	  if (strcmp(type,"max_abs_acceleration") == 0) {

	    const char *cline = json_string_value(json_object_get(response, "cline"));
	    const char *floor = json_string_value(json_object_get(response, "floor"));

	    int nodeTag = this->getNode(cline,floor);	    

	    json_t *theDOFs = json_object_get(response, "dofs");
	    int sizeDOFs = json_array_size(theDOFs);
	    int *dof = new int[sizeDOFs];
	    for (int ii=0; ii<sizeDOFs; ii++)
	      dof[ii] = json_integer_value(json_array_get(theDOFs,ii));

	    string fileString;
	    ostringstream temp;  //temp as in temporary

	    temp << fileBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";

	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();

	    int startTimeSeries = 101;
	    s << "recorder EnvelopeNode -file " << fileName;
	    s << " -timeSeries ";
	    for (int ii=0; ii<sizeDOFs; ii++)
	      s << ii+startTimeSeries << " " ;
	    s << " -node " << nodeTag << " -dof ";
	    for (int ii=0; ii<sizeDOFs; ii++)
	      s << dof[ii] << " " ;
	    s << " accel\n";

	    delete [] dof;
	  }

	  else if (strcmp(type,"max_drift") == 0) {
	    const char * cline = json_string_value(json_object_get(response, "cline"));
	    const char * floor1 = json_string_value(json_object_get(response, "floor1"));
	    const char * floor2 = json_string_value(json_object_get(response, "floor2"));

	    int nodeTag1 = this->getNode(cline,floor1);	    
	    int nodeTag2 = this->getNode(cline,floor2);	    

	    json_t *theDOFs = json_object_get(response, "dofs");
	    int sizeDOFs = json_array_size(theDOFs);
	    int *dof = new int[sizeDOFs];
	    for (int ii=0; ii<sizeDOFs; ii++)
	      dof[ii] = json_integer_value(json_array_get(theDOFs,ii));
	    
	    for (int ii=0; ii<sizeDOFs; ii++) {
	      if (dof[ii] != NDM) { // no drift in vertical dirn
		string fileString1;
		ostringstream temp1;  //temp as in temporary
		
		temp1 << fileBIM << edpEventName << "." << type << "." << cline << "." << floor1 << "." << floor2 << "." << dof[ii] << ".out";
		fileString1=temp1.str(); 
		
		const char *fileName1 = fileString1.c_str();
		
		s << "recorder EnvelopeDrift -file " << fileName1;
		s << " -iNode " << nodeTag1 << " -jNode " << nodeTag2;
		s << " -dof " << dof[ii] << " -perpDirn " << NDM << "\n";
	      }
	    }
	    
	    delete [] dof;
	  }
	  
	  else if (strcmp(type,"residual_disp") == 0) {
	    
	    const char *cline = json_string_value(json_object_get(response, "cline"));
	    const char *floor = json_string_value(json_object_get(response, "floor"));
	    
	    json_t *theDOFs = json_object_get(response, "dofs");
	    int sizeDOFs = json_array_size(theDOFs);
	    int *dof = new int[sizeDOFs];
	    for (int ii=0; ii<sizeDOFs; ii++)
	      dof[ii] = json_integer_value(json_array_get(theDOFs,ii));
	    
	    int nodeTag = this->getNode(cline,floor);	    

	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << fileBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	    fileString=temp.str(); 
	    
	    const char *fileName = fileString.c_str();
	    
	    s << "recorder Node -file " << fileName;
	    s << " -node " << nodeTag << " -dof ";
	    for (int ii=0; ii<sizeDOFs; ii++)
	      s << dof[ii] << " " ;
	    s << " disp\n";
	    
	    delete [] dof;
	  }
	}
	// removing requirement name be same
    }

    // create analysis
    if (analysisType == 1) {
      s << "\n# Perform the analysis\n";
      s << "numberer RCM\n";
      s << "system BandGen\n";
      s << "integrator Newmark 0.5 0.25\n";
      s << "analysis Transient\n";
      s << "analyze " << numSteps << " " << dT << "\n";      
    }
  }
  return 0;
}


// seperate for multi events
int 
OpenSeesPreprocessor::processEvent(ofstream &s, 
				   json_t *event, 
				   int &numPattern, 
				   int &numSeries){
  json_t *timeSeries;
  json_t *pattern;

  const char *eventType = json_string_value(json_object_get(event,"type"));

  std::cerr << "Processing Event type: " << eventType << "\n";

  if (strcmp(eventType,"Seismic") == 0) {
    analysisType = 1;
    numSteps = json_integer_value(json_object_get(event,"numSteps"));
    dT = json_real_value(json_object_get(event,"dT"));
  } else
    return -1;

  std::map <string,int> timeSeriesList;
  std::map <string,int>::iterator it;

  int index = 0;
  json_t *timeSeriesArray = json_object_get(event,"timeSeries");
  int numSeriesArray = json_array_size(timeSeriesArray);
  printf("NUM SERIES: %d\n",numSeriesArray);
  //  json_array_foreach(timeSeriesArray, index, timeSeries) {
  for (int i=0; i<numSeriesArray; i++) {
    timeSeries = json_array_get(timeSeriesArray, i);
					
    //    if (index < NDM) {
      const char *subType = json_string_value(json_object_get(timeSeries,"type"));        
      if (strcmp(subType,"Value")  == 0) {
	double dt = json_real_value(json_object_get(timeSeries,"dT"));
	json_t *data = json_object_get(timeSeries,"data");
	s << "timeSeries Path " << numSeries << " -dt " << dt;
	s << " -values \{ ";
	json_t *dataV;
	int dataIndex;
	json_array_foreach(data, dataIndex, dataV) {
	  s << json_real_value(dataV) << " " ;
	}
	s << " }\n";
	
	string name(json_string_value(json_object_get(timeSeries,"name")));
	printf("TIMESERIES: %s\n",name.c_str());

	timeSeriesList[name]=numSeries;
	numSeries++;
      }
      //    }
  }    

  json_t *patternArray = json_object_get(event,"pattern");
  int numPatternArray = json_array_size(patternArray);

  printf("NUM SERIES: %d\n",numSeriesArray);
  //    json_array_foreach(patternArray, index, pattern) {
  for (int i=0; i<numPatternArray; i++) {
    pattern = json_array_get(patternArray, i);

    const char *subType = json_string_value(json_object_get(pattern,"type"));        
    if (strcmp(subType,"UniformAcceleration")  == 0) {
      int dirn = json_integer_value(json_object_get(pattern,"dof"));
      
      int series = 0;
      string name(json_string_value(json_object_get(pattern,"timeSeries")));
      printf("%s\n",name.c_str());
      it = timeSeriesList.find(name);
      if (it != timeSeriesList.end())
	series = it->second;
      
      if (series == 0) {
	
      }
      
      int seriesTag = timeSeriesList[name];
      s << "pattern UniformExcitation " << numPattern << " " << dirn;
      s << " -accel " << series << "\n";
      numPattern++;
    }
  }

  //  printf("%d %d %f\n",analysisType, numSteps, dT);

  return 0;
}


int
OpenSeesPreprocessor:: getNode(const char * cline,const char * floor){

  int numMapObjects = json_array_size(mapping);
  for (int i=0; i<numMapObjects; i++) {
    json_t *mapObject = json_array_get(mapping, i); 
    const char *c = json_string_value(json_object_get(mapObject,"cline"));
    if (strcmp(c, cline) == 0) {
      const char * f = json_string_value(json_object_get(mapObject,"floor"));
      if (strcmp(f,floor) == 0) {
	int node = json_integer_value(json_object_get(mapObject,"node"));
	printf("cline : %s floor %s node %d\n",cline, floor, node);
	return json_integer_value(json_object_get(mapObject,"node"));
      }
    }
  }
  return -1;
}



int main(int argc, char **argv)
{
  // OpenSeesPreprocessor BIM.json SAM.json EVENT.json SIM.json -getRV
  // OpenSeesPreprocessor BIM.json SAM.json EVENT.json EDP.json SIM.json script 

  OpenSeesPreprocessor thePreprocessor;

  if (argc == 5) 
    thePreprocessor.writeRV(argv[1], argv[2], argv[3], argv[4]);
  else 
    thePreprocessor.createInputFile(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    
  return 0;
}
