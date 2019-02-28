#ifndef _DISTRIBUTION_H_
#define _DISTRIBUTION_H_

#include <string>
#include <vector>

namespace stochastic {

/**
 * Abstract base class for distribution models
 */
class Distribution {
 public:
  /**
   * @constructor Default constructor
   */
  Distribution() = default;

  /**
   * @destructor Virtual destructor
   */
  virtual ~Distribution(){};

  /**
   * Delete copy constructor
   */
  Distribution(const Distribution&) = delete;

  /**
   * Delete assignment operator
   */
  Distribution& operator=(const Distribution&) = delete;

  /**
   * Get the name of the distribution model
   * @return Model name as a string
   */
  virtual std::string name() const = 0;

  /**
   * Compute the cumulative distribution function (CDF) of the distribution at
   * specified input locations
   * @param[in] locations Vector containing locations at which to calculate CDF
   * @return Vector of evaluated values of CDF at input locations
   */
  virtual std::vector<double> cumulative_dist_func(
      const std::vector<double>& locations) const = 0;

  /**
   * Compute the inverse cumulative distribution function (ICDF) of the
   * distribution at specified input locations
   * @param[in] probabilities Vector containing probabilities at which to
   *                          calculate ICDF
   * @return Vector of evaluated values of ICDF at input locations
   */
  virtual std::vector<double> inv_cumulative_dist_func(
      const std::vector<double>& probabilities) const = 0;
};
}  // namespace stochastic

#endif  // _DISTRIBUTION_H_
