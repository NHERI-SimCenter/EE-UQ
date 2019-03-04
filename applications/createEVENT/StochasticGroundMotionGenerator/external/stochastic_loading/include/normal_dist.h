#ifndef _NORMAL_DIST_H_
#define _NORMAL_DIST_H_

#include <string>
#include <vector>
#include <boost/math/distributions/normal.hpp>
#include "distribution.h"

namespace stochastic {
/**
 * Normal distribution
 */
class NormalDistribution : public Distribution {
 public:
  /**
   * @constructor Construct standard normal distribution
   */
  NormalDistribution() = delete;

  /**
   * @constructor Construct normal distribution with specified mean and
   * standard deviation
   * @param[in] mean Mean of distribution
   * @param[in] std_dev Standard deviation of distribution
   */
  NormalDistribution(double mean, double std_dev);

  /**
   * @destructor Virtual destructor
   */
  virtual ~NormalDistribution(){};

  /**
   * Get the name of the distribution model
   * @return Model name as a string
   */
  std::string name() const override { return "NormalDist"; };

  /**
   * Compute the cumulative distribution function (CDF) of the distribution at
   * specified input locations
   * @param[in] locations Vector containing locations at which to
   *                      calculate CDF
   * @return Vector of evaluated values of CDF at input locations
   */
  std::vector<double> cumulative_dist_func(
      const std::vector<double>& locations) const override;

  /**
   * Compute the inverse cumulative distribution function (ICDF) of the
   * distribution at specified input locations
   * @param[in] probabilities Vector containing probabilities at which to
   *                          calculate ICDF
   * @return Vector of evaluated values of ICDF at input locations
   */
  std::vector<double> inv_cumulative_dist_func(
      const std::vector<double>& probabilities) const override;

 protected:
  double mean_;                      /**< Distribution mean */
  double std_dev_;                   /**< Distribution standard deviation */
  boost::math::normal distribution_; /**< Normal distribution */
};
}  // namespace stochastic

#endif  // _NORMAL_DIST_H_
