#include <memory>
#include <string>
#include <stochastic_loading/include/json_object.h>
#include <stochastic_loading/include/factory.h>
#include <stochastic_loading/include/stochastic_model.h>
#include "eq_generator.h"

EQGenerator::EQGenerator(std::string model_name, double moment_magnitude,
                         double rupture_dist, double vs30)
    : {
  eq_model_ =
      Factory<stochastic::StochasticModel, double, double, double, double,
              unsigned int, unsigned int>::instance()
          ->create("VlachosSiteSpecificEQ", std::move(moment_magnitude),
                   std::move(rupture_dist), std::move(vs30), std::move(0.0),
                   std::move(1), std::move(1));
}

EQGenerator::EQGenerator(std::string model_name, double moment_magnitude,
                         double rupture_dist, double vs30, int seed)
    : {
  eq_model_ =
      Factory<stochastic::StochasticModel, double, double, double, double,
              unsigned int, unsigned int, int>::instance()
          ->create("VlachosSiteSpecificEQ", std::move(moment_magnitude),
                   std::move(rupture_dist), std::move(vs30), std::move(0.0),
                   std::move(1), std::move(1), std::move(seed));
}

utilities::JsonObject EQGenerator::generate_time_history(
    const std::string& event_name) {
  return eq_model_->generate(event_name);
}
