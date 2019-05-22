#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
//using namespace std

#include <jansson.h>  // for Json
#ifndef _WIN32
#include <sys/stat.h>
#endif

int main(int argc, const char **argv)
{
  const char *filenameEDP = argv[1];
  const char *filenameOUT = argv[2];

  std::ofstream fOut(filenameOUT);

  json_error_t error;
  json_t *root = json_load_file(filenameEDP, 0, &error);
  json_t *eventsArray = json_object_get(root,"EngineeringDemandParameters");  

  int index1, index2;
  json_t *value1, *value2;

  int numEDP = 0;

  json_array_foreach(eventsArray, index1, value1) {
    json_t *responsesArray = json_object_get(value1,"responses");  
    if (responsesArray != NULL) {
      json_array_foreach(responsesArray, index2, value2) {
	json_t *sType = json_object_get(value2,"scalar_data");
	int numValues = json_array_size(sType);
	for (int i=0; i<numValues; i++) {
	  double value = json_number_value(json_array_get(sType,i));
	  printf("%f ", value);
	  fOut << value << " ";
	  numEDP++;
	}
      }
    }
    responsesArray = json_object_get(value1,"userSpecificResponses");  
    if (responsesArray != NULL) {
      json_array_foreach(responsesArray, index2, value2) {
	json_t *sType = json_object_get(value2,"scalar_data");
	int numValues = json_array_size(sType);
	for (int i=0; i<numValues; i++) {
	  double value = json_number_value(json_array_get(sType,i));
	  printf("%f ", value);
	  fOut << value << " ";
	  numEDP++;
	}
      }
    }
  }

  printf("NUM_EDP= %d\n",numEDP);

  fOut.close();

  //
  // process result files

  if (argc > 5) {
    const char *dakotaIN = argv[3];
    const char *numRF = argv[4];
    int numFiles = atoi(numRF);

    std::cerr << "numFILES: " << numFiles << "\n";

    if (numFiles >= 1) {
      // mkdir
      char copyFile[1000]; 
#if defined(_WIN32)
      std::string dirName("..\\Results\\");
      _mkdir("..\\Results");
#else
      std::string dirName("../Results/");
      mkdir("../Results", 0777);
#endif

      int evalID = -1;
      json_error_t error;
      json_t *rootDakota = json_load_file(dakotaIN, 0, &error);
      json_t *uqMethod = json_object_get(rootDakota,"UQ_Method");  
      if (uqMethod != NULL) {
	std::cerr << "HELLO1";
	json_t *samplingData = json_object_get(uqMethod,"samplingMethodData");
	if (samplingData != NULL) {
	std::cerr << "HELLO2";
	  json_t *evalID_Data = json_object_get(samplingData,"evalID");
	  if (evalID_Data != NULL) {
	    std::cerr << "HELLO3";
	    evalID = json_integer_value(evalID_Data);
	  }
	}
      }
      
      if (evalID != -1) {
	for (int i=0; i<numFiles; i++) {
	  const char *filename = argv[5+i];
	  std::string name(filename);
	  std::string copy = dirName + std::to_string(evalID) + std::string("-") + name;
	  std::cerr << copy << "\n";

	  std::ifstream source(filename, std::ios::binary);
	  std::ofstream dest(copy, std::ios::binary);

	  std::istreambuf_iterator<char> begin_source(source);
	  std::istreambuf_iterator<char> end_source;
	  std::ostreambuf_iterator<char> begin_dest(dest); 
	  std::copy(begin_source, end_source, begin_dest);

	  source.close();
	  dest.close();

	}
      }   
    }
  }

  return 0;
}

