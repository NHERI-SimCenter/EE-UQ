#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

#include <jansson.h>  // for Json

void addEvent(const char *fileNameEvent, json_t *obj, double factor);
void addEvent(const char *fileNameEvent, json_t *obj);

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
    // if --getRV we want to set EVENT file with random variables and events that just contain name
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

      if (strcmp(eventType,"ExistingSimCenterEvents") != 0) {

	json_array_append(newEventArray, value); // copy event for next event app to parse

      } else {

	json_t *eventObj = json_object();
	json_object_set(eventObj,"type", json_string("Seismic"));
	json_object_set(eventObj,"subtype", json_string("MultipleSimCenterEvent"));
	
	json_t *existingEventsArray = json_object_get(value,"Events");
	int numExisting = json_array_size(existingEventsArray);      
	if (numExisting > 1) {
	  json_t *randomVar = json_object();
	  json_object_set(randomVar, "distribution",json_string("discrete_design_set_string"));
	  json_object_set(randomVar, "name",json_string("MultipleEvent"));
	  json_object_set(randomVar, "value",json_string("RV.MultipleEvent"));
	  json_t *theMultipleEvents = json_array();
	  
	  json_t *existingEvent = 0;
	  json_array_foreach(existingEventsArray, index, existingEvent) { 
	    json_array_append(theMultipleEvents, json_object_get(existingEvent,"name"));
	  }

	  json_object_set(randomVar, "elements", theMultipleEvents);
	  json_array_append(rvArray, randomVar);
	  json_object_set(eventObj, "index", json_string("RV.MultipleEvent"));
	
	} else {
	  json_object_set(eventObj, "index", json_integer(0));
	}

	//add first event to event
	json_t *firstEvent = json_array_get(existingEventsArray, 0);
	json_t *fileValue = json_object_get(firstEvent, "fileName");
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
	if ((subType != NULL) && (strcmp("MultipleSimCenterEvent",json_string_value(subType)) ==0)) {

	  json_t *index = json_object_get(value,"index"); 

	  if (index != NULL) {
	    if (json_is_integer(index) == false) {

	      const char *eventName = json_string_value(index);
	      
	      // we need to replace the EVENT with another event
	      json_t *inputEvent = json_array_get(inputEventsArray,count);
	      json_t *events = json_object_get(inputEvent,"Events");
	      for (int i=0; i<json_array_size(events); i++) {
		json_t *theEvent = json_array_get(events, i);
		const char * name = json_string_value(json_object_get(theEvent,"name"));
		json_t *factorValue = json_object_get(theEvent,"factor");
		double factor  = json_number_value(json_object_get(theEvent,"factor"));
		if (strcmp(eventName, name) == 0) {
		  const char *fileName = json_string_value(json_object_get(theEvent,"fileName"));
		  addEvent(fileName, value, factor);
		  i = json_array_size(events);
		}
	      }
	    } else {

	      //
	      // we need to go get factor from input file  and set it in the event
	      //

	      json_t *inputEvent = json_array_get(inputEventsArray,count);
	      json_t *events = json_object_get(inputEvent,"Events");
	      json_t *theEvent = json_array_get(events, 0);
	      double factor  = json_number_value(json_object_get(theEvent,"factor"));

	      /* ********************************************* KEEPING AROUND JUST IN CASE
	      // add factor to timSeries
	      json_t *seriesArray = json_object_get(value,"timeSeries");
	      int countSeries = 0;
	      json_t *seriesValue;
	      json_array_foreach(seriesArray, count, seriesValue) {	      
		json_object_set(seriesValue,"factor",json_real(factor));
	      }
	      ***************************************************************************/
	      json_object_set(value,"factor",json_real(factor));	      
	    }
	    
	  } else {
	    ;
	  }
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

void addEvent(const char *fileName, json_t *obj) {

  // open file and get the first event
  json_error_t error;
  json_t *rootEVENT = json_load_file(fileName, 0, &error);
  json_t *eventsArray = json_object_get(rootEVENT,"Events");  
  json_t *eventToCopy = json_array_get(eventsArray,0);

  // update the object
  json_object_update(obj, eventToCopy); 
}


void addEvent(const char *fileName, json_t *obj, double factor) {

  // open file and get the first event
  json_error_t error;
  json_t *rootEVENT = json_load_file(fileName, 0, &error);
  json_t *eventsArray = json_object_get(rootEVENT,"Events");  
  json_t *eventToCopy = json_array_get(eventsArray,0);
  json_object_set(eventToCopy,"factor",json_real(factor));

  // update the object
  json_object_update(obj, eventToCopy); 
}
