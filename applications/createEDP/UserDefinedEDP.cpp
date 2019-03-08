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

  fprintf(stderr,"HELLO %d\n", argc);

  if (strcmp(argv[argc-1],"--getRV") == 0) { // only do if --getRV is passed

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


    fprintf(stderr,"HELLO\n");

    //
    // for each event we create the edp's
    //

    json_t *eventArray = json_array(); // for each analysis event

    // load SAM and EVENT files
    json_error_t error;

    json_t *rootBIM = json_load_file(filenameBIM, 0, &error);
    json_t *edpData = json_object_get(rootBIM,"EDP");  
    json_t *additionalIn = json_object_get(edpData,"fileNameAI");  
    json_t *postprocess = json_object_get(edpData,"fileNamePS");  

    json_t *edpArray = json_object_get(edpData,"EDP");  

    json_t *rootEVENT = json_load_file(filenameEVENT, 0, &error);
    json_t *eventsArray = json_object_get(rootEVENT,"Events");  
    
    // 
    // parse each event & add response
    //
  
    int index;
    json_t *value;
    
    int numEDP = 0;
    
    json_array_foreach(eventsArray, index, value) {

      fprintf(stderr,"in EVENTS %d\n", index);      

      // check earthquake
      json_t *type = json_object_get(value,"type");  
      const char *eventType = json_string_value(type);
      
      // add the EDP for the event
      json_t *eventObj = json_object();
      
      json_t *name = json_object_get(value,"name"); 
      const char *eventName = json_string_value(name);
      json_object_set(eventObj,"name",json_string(eventName));

      if (additionalIn != NULL) {
	const char *value = json_string_value(additionalIn);
	json_object_set(eventObj,"additionalInput",json_string(value));	
      }

      if (postprocess != NULL) {
	const char *value = json_string_value(postprocess);
	json_object_set(eventObj,"postprocessScript",json_string(value));	
      }

      json_t *responsesArray = json_array(); // for each analysis event
      
      int indexEDP;
      json_t *valueEDP;
      
      json_array_foreach(edpArray, indexEDP, valueEDP) {
	json_t *response = json_object();
	json_t *name = json_object_get(valueEDP,"name"); 
	json_t *dataArray = json_array(); 
	json_object_set(response,"type",name);
	json_object_set(response,"scalar_data",dataArray);
	json_array_append(responsesArray,response);
	numEDP += 1;
      }

      json_object_set(eventObj,"responses",responsesArray);      
      json_array_append(eventArray,eventObj);
    }


    json_object_set(rootEDP,"total_number_edp",json_integer(numEDP));  
    json_object_set(rootEDP,"EngineeringDemandParameters",eventArray);  

    json_dump_file(rootEDP,filenameEDP,0);   
  } 
  return 0;
}
