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

//The Units namespace contains method and data types to help handle unit conversion
namespace Units
{

enum class LengthUnit {Meter = 1, Inch = 2, Centimeter = 3, Millimeter = 4, Foot = 5, Unknown = 0};
enum class TimeUnit {Second = 1, Minute = 2, Hour = 3, Unknown = 0};

//This struct encapsulates a unit system that would have units set for different quantities
struct UnitSystem
{
    LengthUnit lengthUnit = LengthUnit::Unknown;
    TimeUnit timeUnit = TimeUnit::Unknown;
};

//This method parses a string int a length unit enumerator
static LengthUnit ParseLengthUnit(const char* lengthUnitString);

//This method parses a string int a time unit enumerator
static TimeUnit ParseTimeUnit(const char* timeUnitString);

//This method finds the convertion factor from one length unit to another
static double GetLengthFactor(UnitSystem& fromUnit, UnitSystem& toUnit);

//This method finds the convertion factor from one time unit to another
static double GetTimeFactor(UnitSystem& fromUnit, UnitSystem& toUnit);

//This method finds the convertion factor from one acceleration unit to another
static double GetAccelerationFactor(UnitSystem& fromUnit, UnitSystem& toUnit);

}
#endif // OPENSEES_PREPROCESSOR_H
