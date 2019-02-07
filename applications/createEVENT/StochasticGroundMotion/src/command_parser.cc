#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <Clara/include/clara.hpp>
#include "command_parser.h"

CommandParser::CommandParser(int& number_of_arguments, char* arguments[]) {
  command_parser_ =
      clara::detail::Help(configuration_.help).optional() |
      clara::detail::Opt(configuration_.model_name, "Model name")["-modelName"](
          "Name of stochastic model to use")
          .required() |
      clara::detail::Opt(configuration_.moment_magnitude,
                         "Moment magnitude")["-momentMagnitude"](
          "Moment magnitude of earthquake")
          .required() |
      clara::detail::Opt(configuration_.rupture_dist,
                         "Rupture Distance")["-ruptureDist"](
          "Closest-to-site rupture distance in kilometers")
          .required() |
      clara::detail::Opt(configuration_.vs30, "Vs30")["-vs30"](
          "Soil shear wave velocity averaged over top 30 meters in meters per "
          "second")
          .required() |
      clara::detail::Opt(configuration_.output_location,
                         "Event file location")["-filenameEVENT"](
          "Location where generated time history should be stored")
          .required() |
      clara::detail::Opt(configuration_.seed, "Seed value")["-seed"](
          "Seed value that should be used to generate time histories")
          .required();

  auto result = command_parser_.parse(clara::detail::Args(number_of_arguments, arguments));

  // Check whether command line was succussfully parsed
  if (!result) {
    std::cerr << "ERROR: In command line inputs: " << result.errorMessage() << std::endl;
    throw std::invalid_argument("ERROR: In CommandParser::CommandParser: Command line inputs error");
  }

  // If help flag passed, pring usage and exit
  if (configuration_.help) {
    std::cout << command_parser_ << std::endl;
  }
}

double CommandParser::get_magnitude() const {
  return configuration_.moment_magnitude;
}

double CommandParser::get_rupt_dist() const {
  return configuration_.rupture_dist;
}

double CommandParser::get_vs30() const {
  return configuration_.vs30;
}

bool CommandParser::seed_provided() const {
  if (configuration_.seed == "None") {
    return false;
  } else {
    return true;
  }  
}

std::string CommandParser::get_seed() const {
  int seed_value;
  try {
    seed_value = std::stoi(configuration_.seed);
  } catch (const std::exception& e) {
    std::cerr << "ERROR: In CommandParser::get_seed(): Invalid string to int "
                 "conversion. Check input seed value: ";
    << e.what() << std::endl;
  }
  return seed_value;
}

bool CommandParser::get_help_flag() const {
  return configuration_.help;
}

CommandParser::Config CommandParser::get_configuration() const {
  return configuration_;
}
