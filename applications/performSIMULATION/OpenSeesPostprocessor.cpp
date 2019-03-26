
#include "OpenSeesPostprocessor.h"
#include <jansson.h> 
#include <string.h>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>


int main(int argc, char **argv)
{
  if (argc != 5) {
    printf("ERROR %d: correct usage: postprocessOpenSees fileNameBIM fileNameSAM fileNameEVENT filenameEDP\n", argc);
    return -1;
  }

  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  char *filenameEVENT = argv[3];
  char *filenameEDP = argv[4];

  OpenSeesPostprocessor thePostprocessor;

  thePostprocessor.processResults(filenameBIM, filenameEDP);

  return 0;
}


OpenSeesPostprocessor::OpenSeesPostprocessor()
  :filenameEDP(0), filenameBIM(0)
{

}

OpenSeesPostprocessor::~OpenSeesPostprocessor(){
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameBIM != 0)
    delete [] filenameBIM;
}

int 
OpenSeesPostprocessor::processResults(const char *BIM, const char *EDP)
{
  //
  // make copies of filenames in case methods need them
  //

  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameBIM != 0)
    delete [] filenameBIM;

  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  strcpy(filenameEDP,EDP);
  filenameBIM=(char*)malloc((strlen(BIM)+1)*sizeof(char));
  strcpy(filenameBIM,BIM);

  json_error_t error;
  rootEDP = json_load_file(filenameEDP, 0, &error);
  
  processEDPs();

  json_dump_file(rootEDP,filenameEDP,0);
  json_object_clear(rootEDP);  
  return 0;
}


int 
OpenSeesPostprocessor::processEDPs(){

  //
  // foreach EVENT
  //   processEDPs, i.e. open ouputfile, read data, write to edp and dump
  //

  int numTimeSeries = 1;
  int numPatterns = 1;

  int index;
  json_t *event;

  json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  
  
  int numEvents = json_array_size(edps);
  char edpEventName[50];
  
  //
  // try opening results.out file; unknown EDP results may be there or are already in ED"
  //

  ifstream resultsFile;  
  resultsFile.open("results.out");
  double valueResults = 0;

  for (int i=0; i<numEvents; i++) {

    // process event
    json_t *eventEDPs = json_array_get(edps,i);
    const char *eventName = json_string_value(json_object_get(eventEDPs,"name"));
    //const char *edpEventName = json_string_value(json_object_get(eventEDPs,"name"));
    sprintf(edpEventName,"%d",i);    

    json_t *eventEDP = json_object_get(eventEDPs,"responses");
    int numResponses = json_array_size(eventEDP);
    for (int k=0; k<numResponses; k++) {

      json_t *response = json_array_get(eventEDP, k);
      const char *type = json_string_value(json_object_get(response, "type"));

      if (strcmp(type,"max_abs_acceleration") == 0) {
	const char * cline = json_string_value(json_object_get(response, "cline"));
	const char * floor = json_string_value(json_object_get(response, "floor"));
	json_t *dofs = json_object_get(response, "dofs");
	int numDOFs = json_array_size(dofs);

	string fileString;
	ostringstream temp;  //temp as in temporary
	temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	fileString=temp.str(); 
	
	const char *fileName = fileString.c_str();
	
	//
	// opencfile & process data into a json array called: data
	//

	json_t *data = json_array();

	// open file
	ifstream myfile;
	myfile.open (fileName);
	double tmp;

	if (myfile.is_open()) {
	  
	  // read first 2 rows of useless data
	  for (int ii=0; ii<2; ii++)
	    for (int jj=0; jj<numDOFs; jj++) 
	      myfile >> tmp;
	  // read last row and add components to data
	  for (int jj=0; jj<numDOFs; jj++) {
	      myfile >> tmp;
	      json_array_append(data, json_real(tmp));
	  }
	  myfile.close();
	}
	
	// set the response
	json_object_set(response,"scalar_data",data);

      } else if (strcmp(type,"max_rel_disp") == 0) {
	const char * cline = json_string_value(json_object_get(response, "cline"));
	const char * floor = json_string_value(json_object_get(response, "floor"));
	json_t *dofs = json_object_get(response, "dofs");
	int numDOFs = json_array_size(dofs);

	string fileString;
	ostringstream temp;  //temp as in temporary
	temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	fileString=temp.str(); 
	
	const char *fileName = fileString.c_str();
	
	//
	// opencfile & process data into a json array called: data
	//

	json_t *data = json_array();

	// open file
	ifstream myfile;
	myfile.open (fileName);
	double tmp;

	if (myfile.is_open()) {
	  
	  // read first 2 rows of useless data
	  for (int ii=0; ii<2; ii++)
	    for (int jj=0; jj<numDOFs; jj++) 
	      myfile >> tmp;
	  // read last row and add components to data
	  for (int jj=0; jj<numDOFs; jj++) {
	      myfile >> tmp;
	      json_array_append(data, json_real(tmp));
	  }
	  myfile.close();
	}
	
	// set the response
	json_object_set(response,"scalar_data",data);

      } else if (strcmp(type,"max_drift") == 0) {
	
	const char *cline = json_string_value(json_object_get(response, "cline"));
	const char *floor1 = json_string_value(json_object_get(response, "floor1"));
	const char *floor2 = json_string_value(json_object_get(response, "floor2"));
	json_t *dofs = json_object_get(response, "dofs");
	int numDOFs = json_array_size(dofs);
	
	json_t *data = json_array();	
	for (int ii=0; ii<numDOFs; ii++) {
	  int dof = json_integer_value(json_array_get(dofs,ii));
	  string fileString1;
	  ostringstream temp1;  //temp as in temporary
	    
	  temp1 << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor1 << "." << floor2 << "." << dof << ".out";
	  fileString1=temp1.str(); 
	  
	  const char *fileName1 = fileString1.c_str();	
	  
	  // openfile & process data
	  ifstream myfile;
	  myfile.open (fileName1);
	  double absMin, absMax, absValue;
	  
	  absValue = 0.0;
	  if (myfile.is_open()) {
	    myfile >> absMin >> absMax >> absValue;
	    myfile.close();
	  } 
	  json_array_append(data, json_real(absValue));
	}
	
	// set the response
	json_object_set(response,"scalar_data",data);
      }
      
      else if (strcmp(type,"residual_disp") == 0) {

	printf("ERROR - OpenSeesPostprocessor needs to implement\n");
	exit(-1);
	
	const char *cline = json_string_value(json_object_get(response, "cline"));
	const char *floor = json_string_value(json_object_get(response, "floor"));
	
	string fileString;
	ostringstream temp;  //temp as in temporary
	temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	fileString=temp.str(); 
	
	const char *fileName = fileString.c_str();
	
	// openfile & process data
	ifstream myfile;
	myfile.open (fileName);
	double num1 = 0.; 
	double num2 = 0.;
	double num = 0.;
	
	if (myfile.is_open()) {
	  //	    std::vector<double> scores;
	  //keep storing values from the text file so long as data exists:
	  while (myfile >> num1 >> num2) {
	    //	      scores.push_back(num);
	  }
	  
	  // need to process to get the right value, for now just output last
	  num = fabs(num1);
	  if (fabs(num2) > num)
	    num = fabs(num2);
	  
	  myfile.close();
	}
	
	json_object_set(response,"scalar_data",json_real(num));
	/*
	  json_t *scalarValues = json_object_get(response,"scalar_data");
	  json_array_append(scalarValues,json_real(num));
	*/
      } 
      
      else {
	fprintf(stderr, "%s\n",type);
	double valueResults = 0;
	if (resultsFile.is_open() && !resultsFile.eof()) {
	  resultsFile >> valueResults;
	  fprintf(stderr, "%f\n",valueResults);
	}

	json_t *data = json_array();	
	json_array_append(data, json_real(valueResults));
	json_object_set(response,"scalar_data",data);	  

      }
    }
  }

  if (resultsFile.is_open())
      resultsFile.close();

  return 0;
}


