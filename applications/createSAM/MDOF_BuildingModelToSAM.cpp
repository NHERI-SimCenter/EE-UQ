#include <jansson.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <string>
//#include <sstream>
//#include <map>

int
main(int argc, char **argv) {


  char *filenameBIM = NULL;
  char *filenameSAM = NULL;

  // MDOF --filenameBIM file? --filenameEVENT file? --filenameSAM file? <--getRV>

  if (argc == 8 || argc == 7) {

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
        arg++;
    }

  } else {
    fprintf(stderr, "ERROR - MDOF - incorrect # args want: MDOF --filenameBIM file? --filenameEVENT file? --filenameSAM file? <--getRV>\n");
  }


  json_error_t error;
  json_t *rootBIM = json_load_file(filenameBIM, 0, &error);
  json_t *rootSAM = json_object();

  //
  // stuff that we add to the SAM that need to be filled out as we parse the file
  //

  json_t *properties = json_object();
  json_t *geometry = json_object();
  json_t *materials = json_array();
  json_t *nodes = json_array();
  json_t *elements = json_array();
  

  // ensure this is correct type
  json_t *SIM = json_object_get(rootBIM,"StructuralInformation");  

  json_t *typeSIM = json_object_get(SIM,"type");  
  if ((typeSIM == 0) || (strcmp("MDOF_BuildingModel",json_string_value(typeSIM)) != 0)) {
    if (typeSIM != 0) 
      fprintf(stderr, "type: %s\n", json_string_value(typeSIM));
    else
      fprintf(stderr, "type: NONE PROVIDED\n");

    fprintf(stderr, "ERROR - MDOF_BuildingModel - incorrect type\n");    
  }
  
  //
  // get base info that must be there
  //

  json_t *numStoriesType = json_object_get(SIM,"numStories");
  int numStory = json_integer_value(numStoriesType);
  int numNodes = numStory + 1;

  //
  // if no --getRV print out model, otherwise not needed 
  //

  if (argc == 7) {

    // get ModelData
    json_t *modelData = json_object_get(SIM,"ModelData");  

    // initialize some variablles
    double height = 0;
    double weight = 0;
    double G = 386.41;
    int ndf = 2;
    double floorHeight = 0.0;
    double buildingWeight = 0.0;
    int nodeTag = 2;
    int eleTag = 1;
    int matTag = 1;
    double *zeta = new double[numStory];

    //
    // add node at the ground
    //
    json_t *node = json_object();
    json_object_set(node, "name", json_integer(1)); // +2 as we need node at 1 
    json_t *nodePosn = json_array();      
    json_array_append(nodePosn,json_real(0.0));
    json_array_append(nodePosn,json_real(0.0));
    json_object_set(node, "crd", nodePosn);
    json_object_set(node, "ndf", json_integer(ndf));
    json_array_append(nodes,node);


    // add nodes, elements and materials for each floor and story to roof
    int index = 0;
    json_t *floorData;
    json_array_foreach(modelData, index, floorData) {
      double kx = json_number_value(json_object_get(floorData, "kx"));
      double ky = json_number_value(json_object_get(floorData, "ky"));
      double Fyx = json_number_value(json_object_get(floorData, "Fyx"));
      double Fyy = json_number_value(json_object_get(floorData, "Fyy"));
      double bx = json_number_value(json_object_get(floorData, "bx"));
      double by = json_number_value(json_object_get(floorData, "by"));
      double height = json_number_value(json_object_get(floorData, "height"));
      double weight = json_number_value(json_object_get(floorData, "weight"));
      zeta[index]= json_number_value(json_object_get(floorData, "zeta"));

      floorHeight += height;
      double floorMass = weight/G;

      // create a node, element and 2 materials for each story
      json_t *node = json_object();
      json_t *element = json_object();
      json_t *material1 = json_object();
      json_t *material2 = json_object();

      json_object_set(node, "name", json_integer(index+2)); // +2 as we need node at 1 
      json_object_set(node, "mass", json_real(floorMass));
      json_t *nodePosn = json_array();      
      json_array_append(nodePosn,json_real(0));
      json_array_append(nodePosn,json_real(floorHeight));
      json_object_set(node, "crd", nodePosn);
      json_object_set(node, "ndf", json_integer(ndf));

      json_object_set(element, "name", json_integer(eleTag));
      json_object_set(element, "type", json_string("shear_beam2d"));
      json_t *eleMats = json_array();      
      json_array_append(eleMats,json_integer(matTag));
      json_array_append(eleMats,json_integer(matTag+1));
      json_object_set(element, "uniaxial_material", eleMats);
      json_t *eleNodes = json_array();      
      json_array_append(eleNodes,json_integer(index+1));
      json_array_append(eleNodes,json_integer(index+2));
      json_object_set(element, "nodes", eleNodes);
      
      json_object_set(material1,"name",json_integer(matTag));
      json_object_set(material1,"type",json_string("bilinear"));
      json_object_set(material1,"K",json_real(kx));
      json_object_set(material1,"Fy",json_real(Fyx));
      json_object_set(material1,"beta",json_real(bx));

      json_object_set(material2,"name",json_integer(matTag+1));
      json_object_set(material2,"type",json_string("bilinear"));
      json_object_set(material2,"K",json_real(ky));
      json_object_set(material2,"Fy",json_real(Fyy));
      json_object_set(material2,"beta",json_real(by));

      json_array_append(nodes,node);
      json_array_append(materials,material1);
      json_array_append(materials,material2);
      json_array_append(elements, element);

      // increment node, ele and mat tags
      nodeTag++;
      matTag+=2;
      eleTag++;
    }

    json_object_set(properties,"dampingRatio",json_real(zeta[0]));
  }
  
  //
  // now the node mapping array, needed for EDP
  // create array, loop over nodes abd create the mapping
  //

  json_t *mappingArray = json_array();
  int nodeTag = 1;
  char floorString[16];

  fprintf(stderr, "NUM NODES: %d\n",numNodes);

  for (int i=0; i<numNodes; i++) {
      json_t *nodeEntry = json_object();
      json_object_set(nodeEntry,"node",json_integer(nodeTag));
      json_object_set(nodeEntry,"cline",json_string("1"));
      sprintf(floorString,"%d",nodeTag);
      //  itoa(floor, floorString, floor); NOT IN STANDARD
      json_object_set(nodeEntry,"floor",json_string(floorString));

      json_array_append(mappingArray, nodeEntry);
      nodeTag++;
  }

  json_object_set(rootSAM,"NodeMapping",mappingArray);
  json_object_set(rootSAM,"numStory",json_integer(numStory));
  json_object_set(rootSAM,"ndm", json_integer(2));

  json_t *theRVs = json_object_get(SIM,"randomVar");

  json_t *rvObj;
  json_t *rvArray = json_array();
  int index = 0;
  json_array_foreach(theRVs, index, rvObj) {
    json_array_append(rvArray, rvObj);
  }


  json_object_set(rootSAM,"type",json_string("generic"));
  json_object_set(properties,"uniaxialMaterials",materials);
  
  json_object_set(geometry,"nodes",nodes);
  json_object_set(geometry,"elements",elements);

  json_object_set(rootSAM,"Properties",properties);
  json_object_set(rootSAM,"Geometry",geometry);
  json_object_set(rootSAM,"NodeMapping",mappingArray);
  json_object_set(rootSAM,"randomVar",rvArray);

  //
  // dump json to file
  //
  
  json_dump_file(rootSAM,filenameSAM,0);


  return 0;
}
