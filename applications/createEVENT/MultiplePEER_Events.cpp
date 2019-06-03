#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

#include <jansson.h>  // for Json

int addEvent(const char *fileNameEvent, json_t *obj);
int createSimCenterEvent(json_t *obj);

int main(int argc, char **argv)
{
  // StandardEarthquakeEDP --filenameBIM file --filenameEVENT file? <--getRV>

  char *filenameBIM = argv[2];     
  char *filenameEVENT = argv[4]; 

  // create output JSON object for EVENT file and create events array
  json_t *rootEvent = json_object();
  json_t *newEventArray = json_array(); 

  // load INPUT file
  json_error_t error;
  json_t *rootINPUT = json_load_file(filenameBIM, 0, &error);
  json_t *eventsArray = json_object_get(rootINPUT,"Events");  

  if (argc == 6) { 

    //
    // if --getRV:
    //   1) we create Event Files
    //   2) we want to set EVENT file with random variables and events that just contain name
    //

    // output jsonObject for any Random Variables
    json_t *rvArray=json_array();   

    // 
    // parse each event in input:
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

      if (strcmp(eventType,"ExistingPEER_Events") != 0) {

	json_array_append(newEventArray, value); // copy event for next event app to parse

      } else {

	json_t *eventObj = json_object();
	json_object_set(eventObj,"type", json_string("Seismic"));
	json_object_set(eventObj,"subtype", json_string("MultiplePEER_Event"));
	
	json_t *existingEventsArray = json_object_get(value,"Events");
	int numExisting = json_array_size(existingEventsArray);      

	//	json_dump_file(existingEventsArray,"DEBUG-ONE",0);   

	if (numExisting > 1) {

	  json_t *randomVar = json_object();
	  json_object_set(randomVar, "distribution",json_string("discrete_design_set_string"));
	  json_object_set(randomVar, "name",json_string("MultipleEvent"));
	  json_object_set(randomVar, "value",json_string("RV.MultipleEvent"));
	  json_t *theMultipleEvents = json_array();
	  
	  json_t *existingEvent = 0;
	  json_array_foreach(existingEventsArray, index, existingEvent) { 
	    createSimCenterEvent(existingEvent);
	    json_array_append(theMultipleEvents, json_object_get(existingEvent,"name"));
	  }

	  json_object_set(randomVar, "elements", theMultipleEvents);
	  json_array_append(rvArray, randomVar);
	  json_object_set(eventObj, "index", json_string("RV.MultipleEvent"));
	
	} else {

	  json_t *existingEvent = json_array_get(existingEventsArray,0);
	  createSimCenterEvent(existingEvent);	  
	  json_object_set(eventObj, "index", json_integer(0));

	}

	//add first event to event
	json_t *firstEvent = json_array_get(existingEventsArray, 0);
	json_t *fileValue = json_object_get(firstEvent, "name");
	if (fileValue != NULL) {
	  const char *fileName = json_string_value(fileValue);
	  addEvent(fileName, eventObj);
	}

	json_array_append(newEventArray, eventObj);
      }
    }

    // write the variables & events
    json_object_set(rootEvent,"randomVariables",rvArray);
    json_object_set(rootEvent,"Events",newEventArray);
    

    // dump the event file

    json_dump_file(rootEvent,filenameEVENT,0);   
    //    json_dump_file(rootEvent,filenameEVENT,JSON_INDENT(1));   

  }  else { // if not --getRV we want to copy file to EVENT fileName

    //
    // need to open up EVENT file and process to see which of EVENTS to use
    // need to open up INPUT file to see the name of this file (it should be in dir, 
    // then copy file to replace EVENT
    // 

    json_t *rootINPUT = json_load_file(filenameBIM, 0, &error);
    json_t *rootEVENT = json_load_file(filenameEVENT, 0, &error);
    
    // load INPUT file
    json_error_t error;
    json_t *inputEventsArray = json_object_get(rootINPUT,"Events");  
    json_t *eventsEventsArray = json_object_get(rootEVENT,"Events");  
    
    int count;
    json_t *value;
    
    int numEDP = 0;
    
    json_array_foreach(eventsEventsArray, count, value) {
      
      // check earthquake
      json_t *type = json_object_get(value,"type");  
      const char *eventType = json_string_value(type);
      
      if (strcmp(eventType,"Seismic") == 0) {
	json_t *subType = json_object_get(value,"subtype");  
	if ((subType != NULL) && (strcmp("MultiplePEER_Event",json_string_value(subType)) ==0)) {

	  json_t *index = json_object_get(value,"index"); 

	  if (json_is_integer(index) == false) {
	    const char *eventName = json_string_value(index);

	    // we need to replace the EVENT with another event
	    json_t *inputEvent = json_array_get(inputEventsArray,count);
	    json_t *events = json_object_get(inputEvent,"Events");
	    for (int i=0; i<json_array_size(events); i++) {
	      json_t *theEvent = json_array_get(events, i);
	      const char * name = json_string_value(json_object_get(theEvent,"name"));

	      if (strcmp(eventName, name) == 0) {
		addEvent(name, value);
		
		i = json_array_size(events);
	      }
	    }
	  }
	  json_t *eventObj = json_object();
	}	  
      }
    }
    // write rootEvent
    json_dump_file(rootEVENT,filenameEVENT,0);   
  }  

  return 0;
}


//
// procedure to open an existing event file, take first event and "UPDATE"
// the passed obj with the contents of this first event
//

int addEvent(const char *fileName, json_t *obj) {

  // open file and get the first event
  json_error_t error;
  json_t *rootEVENT = json_load_file(fileName, 0, &error);
  json_t *eventsArray = json_object_get(rootEVENT,"Events");  
  json_t *eventToCopy = json_array_get(eventsArray,0);

  // update the object
  json_object_update(obj, eventToCopy); 
  return 0;
}

//
// procedure to create a SimCenter Event given the record in the peerEVENT
//  - the SimCenter Event will be written to a file given by name value

int createSimCenterEvent(json_t *peerEvent) {

  //
  // get name and type 
  //

  // name
  json_t *name = json_object_get(peerEvent,"name");  
  if (name == NULL) {
    cout << "ERROR (createSimEvent): Event does not have a name entry\n";
    json_dump_file(peerEvent,"DEBUG",0);   
    return -1;
  }
  const char *eventName = json_string_value(name);

  // type
  json_t *type = json_object_get(peerEvent,"type");  
  if (type == NULL) {
    cout << "ERROR (createSimEvent): Event " << eventName << " does not have a type entry\n";
    return -1;
  }
  const char *eventType = json_string_value(type);

  if (strcmp(eventType,"PeerEvent") != 0) {
    cout << "ERROR (createSimEvent): Event type : " << eventType << " is not of type \"PeerEvent\" \n";
    return -1;
  }

  //
  // for each PEER Event to be added
  //   we need to create a timeSeries and a pattern from data in PEER input file
  //     timeSeries contains fields for name, type, dT, numPoints, data
  //     pattern contains type, timSeries, dof
  //   this timeSeries and pattern are placed in the events timeSeries and pattern array
  //

  // arrays for timeSeries and pattern
  json_t *timeSeriesArray = json_array();
  json_t *patternArray = json_array();

  // get the Records array, which contains list of the PeerEvents to be added
  json_t *recordsArray = json_object_get(peerEvent,"Records");
  json_t *peerRecord = 0;
  int index;

  // loop over ths array of PEER events, creating a timeSeries and pattern for the event
  double dT =0.0;
  int numPoints =0;
  json_array_foreach(recordsArray, index, peerRecord) { 

    //    json_t *eventObject = json_object();

    // the PEER events timeSeries and pattern object
    json_t *timeseriesObj = json_object();
    json_t *patternObj = json_object();

    // the file
    json_t *fileNameType = json_object_get(peerRecord,"fileName");  
    if (type == NULL) {
      cout << "ERROR (createSimEvent): Event " << index  << " in " << eventName 
	   << " does not have a type entry\n";
      return -1;
    }
    const char *fileName = json_string_value(fileNameType);

    json_t *factorObj = json_object_get(peerRecord,"factor");

    dT = 0.0;
    numPoints = 0;

    //
    // open PEER File & Process
    //   - get deltaT and numPoints
    //   - read data points, placing into json array
    //   - fill in event object
    //

    // open file

    ifstream fileIn(fileName);

    // read dT and numPoints

    bool foundT = false;
    string word;
    string line;
    while(!fileIn.eof() && foundT == false && fileIn >> word) {

      if (word.compare("NPTS=") == 0) {
	foundT = true;
	string NPTS;
	string data;
	foundT = true;
	fileIn >> NPTS;
	fileIn >> data;
	fileIn >> dT;

	NPTS.pop_back(); // remove the comma
	stringstream daft(NPTS);
	daft >> numPoints;
	cout << "file: " << fileName << " NPTS= " << NPTS << " dT= " << numPoints << " " << dT << "\n";
	
	// get rest of line
	getline(fileIn,line);
      }
    }
    if (dT == 0 || numPoints == 0) {
      cout << "ERROR Parsing PEER Record:check format of file: " << fileName;
      return -2;
    }

    // create a json array to hold the data
    json_t *dataArray = json_array();

    // read the values from file and place into json data array
    double number =0;
    while (!fileIn.eof()) {
      fileIn >> number;
      json_array_append(dataArray, json_real(number));
    }

    //
    // depending on dirn set data for name of timeSeries, use that name and dirn in pattern
    //

    json_t *dirnType = json_object_get(peerRecord,"dirn");  
    int dirn = json_integer_value(dirnType);

    if (dirn == 1) {
      json_object_set(timeseriesObj,"name",json_string("dirn1"));
      json_object_set(patternObj,"timeSeries",json_string("dirn1"));
    } else if (dirn == 2) {
      json_object_set(timeseriesObj,"name",json_string("dirn2"));
      json_object_set(patternObj,"timeSeries",json_string("dirn2"));
    } else if (dirn == 3) {
      json_object_set(timeseriesObj,"name",json_string("dirn3"));
      json_object_set(patternObj,"timeSeries",json_string("dirn3"));
    } else {
      cout << "ERROR: dirn " << dirn << " in PEER EVENT not valid\n";
      return -1;
    }

    //
    // now we have all the data, fill in the events timeSeries and pattern objects
    //

    json_object_set(timeseriesObj,"type",json_string("Value"));
    json_object_set(patternObj,"type",json_string("UniformAcceleration"));
    json_object_set(patternObj,"dof",json_integer(dirn));
    json_object_set(timeseriesObj,"factor",factorObj);

    json_object_set(timeseriesObj,"dT",json_real(dT));
    json_object_set(timeseriesObj,"numSteps",json_integer(numPoints));
    json_object_set(timeseriesObj,"data",dataArray);

    //
    // append the timeSeries and pattern objects to SimCenter timeSeries and pattern array
    //

    json_array_append(timeSeriesArray, timeseriesObj);
    json_array_append(patternArray, patternObj);
  }

  //
  // create and fill in the SimCenter EVENT
  //

  // create the event
  json_t *eventObj = json_object();

  // fill it in
  json_object_set(eventObj,"name",name);
  json_object_set(eventObj,"type",json_string("Seismic"));
  json_object_set(eventObj,"dT",json_real(dT));
  json_object_set(eventObj,"numSteps",json_integer(numPoints));
  json_object_set(eventObj,"timeSeries",timeSeriesArray);
  json_object_set(eventObj,"pattern", patternArray);

  //
  // now the EVENT needs to be added to an Events array in the actual SimCenterEvent
  //  as each SimCenterEvent can be used by itself, we create this Events array  and place in the output obj
  //

  json_t *eventsArray = json_array();
  json_array_append(eventsArray, eventObj);


  json_t *outputObj = json_object();
  json_object_set(outputObj,"Events",eventsArray);

  //
  // write a SimCenter Event file by dumping outputObj JSON to a file
  //

  //  json_dump_file(outputObj, eventName, JSON_INDENT(1));
  json_dump_file(outputObj, eventName, JSON_COMPACT);

  //
  // clear memory
  //

  json_object_clear(outputObj);

  // done!
  return 0;
}
