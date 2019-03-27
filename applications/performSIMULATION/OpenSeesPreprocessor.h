#ifndef OPENSEES_PREPROCESSOR_H
#define OPENSEES_PREPROCESSOR_H
class json_t;
#include <fstream>
#include <map>
#include <string>

using namespace::std;

class OpenSeesPreprocessor {

 public:
  OpenSeesPreprocessor();
  ~OpenSeesPreprocessor();

  int writeRV(const char *BIM,
	      const char *SAM,
	      const char *EVENT,
	      const char *SIM);

  int createInputFile(const char *BIM,
		      const char *SAM,
		      const char *EVENT,
		      const char *EDP,
		      const char *SIM,
		      const char *tcl);

  int processMaterials(ofstream &out);
  int processSections(ofstream &out);
  int processNodes(ofstream &out);
  int processElements(ofstream &out);
  int processDamping(ofstream &out);
  int processEvents(ofstream &s);
  int processEvent(ofstream &s, 
		   json_t *event, 
		   int &numPattern, 
		   int &numSeries);

  int getNode(const char *cline, const char *floor);

 private:
  const char *fileBIM;
  const char *fileSAM;
  const char *fileEVENT;
  const char *fileEDP;
  const char *fileSIM;

  json_t *rootBIM;
  json_t *rootSIM;
  json_t *rootSAM;
  json_t *rootEDP;
  json_t *rootEVENT;
  json_t *mapping;

  int analysisType;
  int numSteps;
  double dT;
  int nStory;   //number of stories

  int NDM;
  int NDF;
};

#endif // OPENSEES_PREPROCESSOR_H
