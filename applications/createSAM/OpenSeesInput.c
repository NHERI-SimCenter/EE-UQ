#include <jansson.h> 
#include <stdlib.h>
#include <stdio.h>
//#include <string>
//#include <sstream>
//#include <map>

int
main(int argc, char **argv) {

  // OpenSeesInput --filenameBIM file? --filenameEVENT file? --filenameSAM file? --filePath path? --fileName file? <--getRV>

  if (argc == 12) {

    char *filenameBIM = NULL;
    // NOT USED: char *filenameEVENT = argv[4]; 
    char *filenameSAM = NULL;
    // NOT USED: char *filePath = argv[8];
    char *fileName = NULL;
    

    int arg = 1;
    while(arg < argc)
    {
        if (strcmp(argv[arg], "--filenameBIM") == 0)
        {
            arg++;
            filenameBIM = argv[arg];
        }
        else if (strcmp(argv[arg], "--filenameSAM") == 0)
        {
            arg++;
            filenameSAM = argv[arg];
        }
        else if (strcmp(argv[arg], "--fileName") == 0)
        {
            arg++;
            fileName = argv[arg];
        }
        arg++;
    }


    json_error_t error;
    json_t *rootBIM = json_object();
    json_t *rootSAM = json_object();

    json_object_set(rootSAM,"mainScript",json_string(fileName));
    json_object_set(rootSAM,"type",json_string("OpenSeesInput"));

    //
    // now the node mapping array, needed for EDP
    // create array, lop over nodes in list and fill in
    //

    json_t *mappingArray = json_array();

    //
    // load input file & get the array of nodes
    //

    rootBIM = json_load_file(filenameBIM, 0, &error);
    json_t *theSIM = json_object_get(rootBIM,"StructuralInformation");

    json_t *theNodes = json_object_get(theSIM,"nodes");

    // check nodes exists
    if (theSIM == NULL ||  theNodes == NULL) {
      fprintf(stderr,"OpenSeesInput - no nodes section found ");    
      return -1;
    }
    
    // loop over each node in nodes list, creating nodeMapping entry
    int index;
    json_t *intObj;
    int floor = 1;
    char floorString[10];
    
    json_array_foreach(theNodes, index, intObj) {
      json_t *nodeEntry = json_object();
      int tag = json_integer_value(intObj);
      json_object_set(nodeEntry,"node",json_integer(tag));
      json_object_set(nodeEntry,"cline",json_string("1"));
      sprintf(floorString,"%d",floor);
      //  itoa(floor, floorString, floor); NOT IN STANDARD
      json_object_set(nodeEntry,"floor",json_string(floorString));
      floor++;
      json_array_append(mappingArray, nodeEntry);
    }

    json_object_set(rootSAM,"NodeMapping",mappingArray);

    // add #story and ndm
    int nStory = floor -2;
    json_object_set(rootSAM,"numStory",json_integer(nStory));

    json_t *ndm = json_object_get(theSIM, "ndm");
    json_object_set(rootSAM,"ndm", ndm);


    json_t *theRVs = json_object_get(theSIM,"randomVar");

    // check nodes exists
    if (theSIM == NULL ||  theRVs == NULL) {
      fprintf(stdout,"OpenSeesInput - no randomVar section found ");    
      return -1;
    }
    
    // loop over each node in nodes list, creating nodeMapping entry
    json_t *rvObj;
    json_t *rvArray = json_array();
    json_array_foreach(theRVs, index, rvObj) {
      
      //      json_t *nodeEntry = json_object();
      //      int tag = json_integer_value(intObj);
      json_array_append(rvArray, rvObj);
    }
    json_object_set(rootSAM,"randomVar",rvArray);

    //
    // dump json to file
    //

    json_dump_file(rootSAM,filenameSAM,0);
  }

  return 0;
}
