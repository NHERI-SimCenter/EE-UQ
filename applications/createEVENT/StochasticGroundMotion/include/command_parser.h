#ifndef _COMMAND_PARSER_H
#define _COMMAND_PARSER_H

#include <cctype>
#include <string>
#include <vector>
#include <Clara/include/clara.hpp>

/**
 * Class for processing command line arguments
 */
class CommandParser {
 public:
  /**
   * @constructor Default contstructor
   */
  CommandParser() = default;
  
  /**
   * @constructor Create a new command parser
   * @param[in] number_of_arguments Number of command line arguments
   * @param[in] arguments Command line arguments
   */
  CommandParser(int& number_of_arguments, char* arguments[]);

  /**
   * Structure for storing configuration
   */
  struct Config {
    std::string model_name; /**< Name of stochastic model to use */
    double moment_magnitude; /**< Moment magnitude of event */
    double rupture_dist; /**< Closest-to-site rupture distance in kilometers */
    double vs30; /**< Soil shear wave velocity averaged over top 30 meters in
		    meters per second */
    std::string seed =
        "None"; /**< Seed value to use for random number generator */
    std::string event_file; /**< Location where event should be saved */
    bool help = false; /**< Flag indicating whether the input requests the help
                          option */
  };

  /**
   * Get the moment magnitude of the earthquake
   * @return Moment magnitude of earthquake
   */
  double get_magnitude() const;

  /**
   * Get the rupture distance of site from earthquake
   * @return Rupture distance for site
   */
  double get_rupt_dist() const;

  /**
   * Get vs30 for the site
   * @return Vs30 for site
   */
  double get_vs30() const;

  /**
   * Get whether seed value provided
   */
  bool seed_provided() const;

  /**
   * Get the seed value that was provided as an integer
   * @return Seed value as integer
   */
  int get_seed() const;
  
  /**
   * Get file name that event should be stored to
   * @return Location of event file
   */
  std::string get_event_file() const;

  /**
   * Get flag that indicates whether help flag was passed in command line
   * @return Flag indicating whether help was called
   */
  bool get_help_flag() const;

  /**
   * Get the configuration structure generated from command line inputs
   * @return Struct containing configuration
   */
  Config get_configuration() const;
  
 private:
  /** Struct that saves all command line inputs */
  Config configuration_;
  /**< Clara command line object for parsing inputs */
  clara::detail::Parser command_parser_;
};

#endif  // _COMMAND_PARSER_H
