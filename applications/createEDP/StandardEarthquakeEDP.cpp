#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

#include <jansson.h>  // for Json

int main(int argc, char **argv)
{

  // StandardEarthquakeEDP --filenameBIM file --filenameEVENT file? --filenameSAM file? --filenameEDP file? <--getRV>

  if (argc == 10) { // only do if --getRV is passed

    char *filenameBIM = argv[2];     
    char *filenameEVENT = argv[4]; 
    char *filenameSAM   = argv[6];
    char *filenameEDP   = argv[8];

    /********************************************************
                       REMOVED FOR SPEED
    //
    // parse args
    // 
    
    int arg = 1;
    while (arg < argc) {
      if (strcmp(argv[arg], "--filenameBIM") ==0) {
	arg++;
	filenameBIM = argv[arg];
      }
      else if (strcmp(argv[arg], "--filenameEVENT") ==0) {
	arg++;
	filenameEVENT = argv[arg];
      }
      else if (strcmp(argv[arg], "--filenameSAM") ==0) {
	arg++;
	filenameSAM = argv[arg];
      }
      else if (strcmp(argv[arg], "--filenameEDP") ==0) {
	arg++;
	filenameEDP = argv[arg];
      }
      else if (strcmp(argv[arg], "--getRV") ==0) {
	getRV = true;
      }
      
      arg++;
    }
    if (getRV == true) 
      std::cerr << "GETRV - TRUE\n";
    else
      std::cerr << "GETRV - FALSE\n";

    //
    // if not all args present, exit with error
    //

    if (filenameEDP == 0 || 
        filenameEVENT == 0 ||
        filenameSAM == 0 ||
        filenameBIM == 0) {

      std::cerr << "ERROR - missing input args\n";
      exit(-1);
    }
    *****************************************************************/


    // create output JSON object
    json_t *rootEDP = json_object();

    // place an empty random variable field
    json_t *rvArray=json_array();   
    json_object_set(rootEDP,"RandomVariables",rvArray);

    //
    // for each event we create the edp's
    //

    json_t *eventArray = json_array(); // for each analysis event

    // load SAM and EVENT files
    json_error_t error;

    json_t *rootEVENT = json_load_file(filenameEVENT, 0, &error);
    json_t *eventsArray = json_object_get(rootEVENT,"Events");  
    
    json_t *rootSAM = json_load_file(filenameSAM, 0, &error);
    json_t *mappingArray = json_object_get(rootSAM,"NodeMapping"); 
    json_t *theNDM = json_object_get(rootSAM,"ndm");  
    int ndm = json_integer_value(theNDM);
    
    // 
    // parse each event:
    //  1. make sure earthquake
    //  2. add responses
    //
  
    int index;
    json_t *value;
    
    int numEDP = 0;
    
    json_array_foreach(eventsArray, index, value) {
      
      // check earthquake
      json_t *type = json_object_get(value,"type");  
      const char *eventType = json_string_value(type);
      
      //      fprintf(stderr, "EventType: %s\n", eventType);

      if (strcmp(eventType,"Seismic") != 0) {
	json_object_clear(rootEVENT);
	printf("WARNING event type %s not Seismic NO OUTPUT", eventType);
      }
      
      // add the EDP for the event
      json_t *eventObj = json_object();
      
      json_t *name = json_object_get(value,"name"); 
      const char *eventName = json_string_value(name);
      json_object_set(eventObj,"name",json_string(eventName));

      // fprintf(stderr, "EventName: %s\n", eventName);

      //
      // create a json_array of ints indicating what dof the event patterns apply to
      //  -- neeeded in EPD

      int numDOF = 0;
      json_t *theDOFs = json_array();
      int *tDOFs = 0;
      json_t *patternArray = json_object_get(value,"pattern");
      int numPattern = json_array_size(patternArray);
      tDOFs = new int[numPattern];

      if (numPattern != 0) {
	for (int ii=0; ii<numPattern; ii++) {
	  json_t *thePattern = json_array_get(patternArray, ii);
	  json_t *theDof = json_object_get(thePattern, "dof");
	  tDOFs[ii] = json_integer_value(theDof);
	  if (theDof != 0) {
	    json_array_append(theDOFs, theDof);
	    numDOF++;
	  } else {
	    printf("ERROR no dof with Seismic event pattern %d", ii);
	    exit(-1);
	  }
	}
      } else {
	printf("ERROR no patterns with Seismic event");
	exit(-1);
      }
      
      //    json_dump_file(eventObj,"TEST",0);
      
      json_t *responsesArray = json_array(); // for each analysis event
      
      // create responses for floor accel and story drift 
      
      int mapIndex1;
      json_t *value1;
      
      int count = 0;
      const char *floor1 = 0;
      const char *cline = 0;
      const char *floor = 0;

      json_array_foreach(mappingArray, mapIndex1, value1) {
	
	cline = json_string_value(json_object_get(value1,"cline"));
	floor = json_string_value(json_object_get(value1,"floor"));
	int node = json_integer_value(json_object_get(value1,"node"));
	
	// floor abs acceleration
	json_t *responseA = json_object();
	json_object_set(responseA,"type",json_string("max_abs_acceleration"));      
	json_object_set(responseA,"cline",json_string(cline));
	json_object_set(responseA,"floor",json_string(floor));
	json_object_set(responseA,"dofs",theDOFs);
	json_t *dataArrayA = json_array(); 
	json_object_set(responseA,"scalar_data",dataArrayA);
	json_array_append(responsesArray,responseA);
	numEDP += numDOF;

	// floor relative disp
	json_t *responseD = json_object();
	json_object_set(responseD,"type",json_string("max_rel_disp"));      
	json_object_set(responseD,"cline",json_string(cline));
	json_object_set(responseD,"floor",json_string(floor));
	json_object_set(responseD,"dofs",theDOFs);
	json_t *dataArrayD = json_array(); 
	json_object_set(responseD,"scalar_data",dataArrayD);
	json_array_append(responsesArray,responseD);
	numEDP += numDOF;

	// interstory drift
	if (count > 0) {
	  json_t *response = json_object();
	  json_object_set(response,"type",json_string("max_drift"));      
	  json_object_set(response,"cline",json_string(cline));
	  json_object_set(response,"floor1",json_string(floor1));
	  json_object_set(response,"floor2",json_string(floor));

	  // we a=cannot just add dof's as before in case vertical
	  json_t *dofArray = json_array();
	  for (int i=0; i<numDOF; i++) {
	    if (tDOFs[i] != ndm) {
	      json_array_append(dofArray, json_integer(tDOFs[i]));
	      numEDP++;
	    }
	  }
	  json_object_set(response,"dofs",dofArray);

	  json_t *dataArray = json_array(); 
	  json_object_set(response,"scalar_data",dataArray);
	  json_array_append(responsesArray,response);

	}

	floor1 = floor;
	count++;
      }
      /*
	json_t *response = json_object();
	json_object_set(response,"type",json_string("residual_disp"));      
	json_object_set(response,"cline",json_integer(1));
	json_object_set(response,"floor",json_integer(numStory+1));
	json_t *dataArray = json_array(); 
	json_object_set(response,"scalar_data",dataArray);
	json_array_append(responsesArray,response);
	numEDP++;
      */      
      json_object_set(eventObj,"responses",responsesArray);
      
      json_array_append(eventArray,eventObj);

      if (tDOFs != 0)
	delete [] tDOFs;
    }


    json_object_set(rootEDP,"total_number_edp",json_integer(numEDP));  
    json_object_set(rootEDP,"EngineeringDemandParameters",eventArray);  

    json_dump_file(rootEDP,filenameEDP,0);   
  } 
  return 0;
}
