#ifndef _STOCHASTIC_MODEL_H_
#define _STOCHASTIC_MODEL_H_

#include <string>
#include "json_object.h"

namespace stochastic {

/**
 * Abstract base class for stochastic models
 */
class StochasticModel {
 public:
  /**
   * @constructor Default constructor
   */
  StochasticModel() = default;

  /**
   * @destructor Virtual destructor
   */
  virtual ~StochasticModel() {};

  /**
   * Delete copy constructor
   */
  StochasticModel(const StochasticModel&) = delete;

  /**
   * Delete assignment operator
   */
  StochasticModel& operator=(const StochasticModel&) = delete;

  /**
   * Get the name of the stochastic model
   * @return Model name as a string
   */
  std::string model_name() const { return model_name_; };

  /**
   * Generate loading based on stochastic model and store
   * outputs as JSON object
   * @param[in] event_name Name to assign to event
   * @param[in] g_units Indicates that time histories should be returned in
   *                    units of g. Defaults to false where time histories
   *                    are returned in units of m/s^2
   * @return JsonObject containing loading time histories
   */
  virtual utilities::JsonObject generate(const std::string& event_name,
                                         bool g_units = false) = 0;

  /**
   * Generate loading based on stochastic model and write
   * results to file in JSON format
   * @param[in] event_name Name to assign to event
   * @param[in, out] output_location Location to write outputs to
   * @param[in] g_units Indicates that time histories should be returned in
   *                    units of g. Defaults to false where time histories
   *                    are returned in units of m/s^2
   * @return Returns true if successful, false otherwise
   */
  virtual bool generate(const std::string& event_name,
                        const std::string& output_location,
                        bool g_units = false) = 0;

 protected:
  std::string model_name_ = "StochasticModel"; /**< Name of stochastic model */  
};
}  // namespace stochastic

#endif  // _STOCHASTIC_MODEL_H_
