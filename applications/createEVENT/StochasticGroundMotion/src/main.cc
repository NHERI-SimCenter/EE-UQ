#include <fstream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <stochastic_loading/include/configure.h>
#include <stochastic_loading/include/json_object.h>
#include "command_parser.h"
#include "eq_generator.h"

using json = nlohmann::json;

int main(int argc, char** argv) {

  // Read command line arguments
  CommandParser inputs;
  try {
    inputs = CommandParser(argc, argv);    
  } catch (const std::invalid_argument& e) {
    std::cerr << "\nException caught in command line processing: " << e.what() << std::endl;
    return 1;
  }
  
  // Check if help menu requested
  if (inputs.get_help_flag()) {
    return 0;
  }

  // Initialize stochastic load generation library
  config::initialize();
  
  // Check if random variable flag has been passed
  if (inputs.get_rv_flag()) {
    json event;
    event.emplace("randomVariables", json::array());

    // If seed has been provided, generate time history
    if (inputs.seed_provided()) {
      try {
        auto eq_generator = EQGenerator(
            inputs.get_model_name(), inputs.get_magnitude(),
            inputs.get_rupt_dist(), inputs.get_vs30(), inputs.get_seed());

	auto time_history = eq_generator.generate_time_history("StochasticMotion");
	json time_series;
        time_series.emplace(
            {{"name", time_history.get_value<std::string>("name")},
             {"type", time_history.get_value<std::string>("type")},
             {"dT", time_history.get_value<double>("dT")},
             {"data", time_history.get_value<std::vector<double>>("data")}});

	// CONTINUE HERE
	
        // GENERATE TIME HISTORY WITH SEED HERE AND ADD TO EVENT FILE
      } catch (const std::exception& e) {
        std::cerr << "ERROR: In main() of StochasticGroundMotion with getRV "
                     "flag set: "
                  << e.what() << std::endl;
        return 1;
      }
      // No seed provided, so only place type and subtype in Events
    } else {
      event.emplace("Events", json::array({{"type", "Seismic"}, {"subtype", "StochasticGroundMotion"}}));
    }

    std::ofstream event_file;
    event_file.open(inputs.get_event_file());

    if (!event_file.is_open()) {
      std::cerr << "\nERROR: In main() of StochasticGroundMotion: Could "
                   "not open output location\n";
    }

    // Write prettyfied JSON to file
    event_file << std::setw(4) << event << std::endl;
    event_file.close();

    if (event_file.fail()) {
      std::cerr
          << "\nERROR: In In main() of StochasticGroundMotion:: Error when "
             "closing output location\n";
    }
  // No random variable flag passed, so generate ground motion
  } else {
    // If seed is not provided, need to generate new ground motion. Otherwise,
    // don't need to do anything since ground motion was generated when getRV
    // flag was passed
    if (!inputs.seed_provided()) {
      try {
        auto eq_generator = EQGenerator(
            inputs.get_model_name(), inputs.get_magnitude(),
            inputs.get_rupt_dist(), inputs.get_vs30(), inputs.get_seed());
        // GENERATE TIME HISTORY WITH SEED HERE AND ADD TO EVENT FILE
      } catch (const std::exception& e) {
        std::cerr
            << "ERROR: In main() of StochasticGroundMotion without getRV flag: "
            << e.what() << std::endl;
        return 1;
      }
    }
  }

  return 0;
}
