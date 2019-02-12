#include <fstream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <configure.h>
#include <nlohmann_json/json.hpp>
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

        auto time_history =
            eq_generator.generate_time_history("StochasticMotion")
                .get_library_json();

        if (time_history.at("Events").size() != 1) {
          throw std::runtime_error(
              "ERROR: In main() of StochasticGroundMotion with getRV "
              "flag set: Generated events should have length 1\n");
        }
        auto event_data = time_history.at("Events")[0];

        auto array_entry = json::object(
            {{"name", event_data.at("name")},
             {"type", event_data.at("type")},
             {"dT", event_data.at("dT")},
             {"Data", "Time history generated using " +
                          inputs.get_model_name() + " model"},
             {"numSteps", event_data.at("numSteps")},
             {"timeSeries", json::array({event_data.at("timeSeries")[0]})},
             {"pattern", json::array({event_data.at("pattern")[0]})}});
	
        auto event_array = json::array();
        event_array.push_back(array_entry);
        event.emplace("Events", event_array);
      } catch (const std::exception& e) {
        std::cerr << "ERROR: In main() of StochasticGroundMotion with getRV "
                     "flag set: "
                  << e.what() << std::endl;
        return 1;
      }
      // No seed provided, so only place type and subtype in Events
    } else {
      json event_description;
      event_description.emplace("type", "Seismic");
      event_description.emplace("subtype", "StochasticGroundMotion");

      auto pattern = json::object({{"dof", 1},
                                   {"timeSeries", "accel_x"},
                                   {"type", "UniformAcceleration"}});

      event_description.emplace("pattern", json::array({pattern}));
      event.emplace("Events", json::array({event_description}));
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
      json event;
      event.emplace("randomVariables", json::array());
      try {
        auto eq_generator =
            EQGenerator(inputs.get_model_name(), inputs.get_magnitude(),
                        inputs.get_rupt_dist(), inputs.get_vs30());

        auto time_history =
            eq_generator.generate_time_history("StochasticMotion")
                .get_library_json();


        if (time_history.at("Events").size() != 1) {
          throw std::runtime_error(
              "ERROR: In main() of StochasticGroundMotion with getRV "
              "flag set: Generated events should have length 1\n");
        }
        auto event_data = time_history.at("Events")[0];

        auto array_entry = json::object(
            {{"name", event_data.at("name")},
             {"type", event_data.at("type")},
             {"dT", event_data.at("dT")},
             {"Data", "Time history generated using " +
                          inputs.get_model_name() + " model"},
             {"numSteps", event_data.at("numSteps")},
             {"timeSeries", json::array({event_data.at("timeSeries")[0]})},
             {"pattern", json::array({event_data.at("pattern")[0]})}});
	
        auto event_array = json::array();
        event_array.push_back(array_entry);
        event.emplace("Events", event_array);
      } catch (const std::exception& e) {
        std::cerr
            << "ERROR: In main() of StochasticGroundMotion without getRV flag: "
            << e.what() << std::endl;
        return 1;
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
    }
  }

  return 0;
}
