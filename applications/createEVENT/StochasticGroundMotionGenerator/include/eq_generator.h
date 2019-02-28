#ifndef _EQ_GENERATOR_H_
#define _EQ_GENERATOR_H_

#include <string>
#include <vector>
#include <stochastic_loading/include/json_object.h>
#include <stochastic_loading/include/stochastic_model.h>

/**
 * Class for generating time histories for particular earthquake event
 */
class EQGenerator {
 public:
  /**
   * @constructor Delete default constructor
   */
  EQGenerator() = delete;

  /**
   * @constructor Construct earthquake generator for earthquake at input site
   * location with input moment magnitude
   * @param[in] model_name Name of stochastic model
   * @param[in] moment_magnitude Moment magnitude for earthquake event
   * @param[in] rupture_dist Closest-to-site rupture distance in kilometers
   * @param[in] vs30 Soil shear wave velocity averaged over top 30 meters in
   *                 meters per second
   */
  EQGenerator(std::string model_name, double moment_magnitude, double rupture_dist, double vs30);
  
  /**
   * @constructor Construct earthquake generator for earthquake at input site
   * location with input moment magnitude
   * @param[in] model_name Name of stochastic model
   * @param[in] moment_magnitude Moment magnitude for earthquake event
   * @param[in] rupture_dist Closest-to-site rupture distance in kilometers
   * @param[in] vs30 Soil shear wave velocity averaged over top 30 meters in
   *                 meters per second
   */
  EQGenerator(std::string model_name, double moment_magnitude,
              double rupture_dist, double vs30, int seed);

  /**
   * Generate time history based on model parameters
   * @param[in] event_name Name to use for event
   * @return JsonObject containing loading time histories
   */
  utilities::JsonObject generate_time_history(const std::string& event_name);

 private:
  std::shared_ptr<stochastic::StochasticModel> eq_model_; /**< Stochastic model
                                                             to generate
                                                             earthquake time
                                                             histories */
};

#endif // _EQ_GENERATOR_H_
