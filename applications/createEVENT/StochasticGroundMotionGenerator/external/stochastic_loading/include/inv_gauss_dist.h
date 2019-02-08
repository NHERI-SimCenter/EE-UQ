#ifndef _INV_GAUSS_DIST_H_
#define _INV_GAUSS_DIST_H_

#include <string>
#include <vector>
#include <boost/math/distributions/inverse_gaussian.hpp>
#include "distribution.h"

namespace stochastic {
/**
 * Inverse Gaussian distribution
 */
class InverseGaussianDistribution : public Distribution {
 public:
  /**
   * @constructor Delete default constructor
   */
  InverseGaussianDistribution() = delete;

  /**
   * @constructor Construct inverse Gaussian distribution with specified mean and
   * standard deviation
   * @param[in] mean Mean of distribution
   * @param[in] scale Scale parameter of distribution
   */
  InverseGaussianDistribution(double mean, double scale);

  /**
   * @destructor Virtual destructor
   */
  virtual ~InverseGaussianDistribution(){};

  /**
   * Get the name of the distribution model
   * @return Model name as a string
   */
  std::string name() const override { return "InverseGaussianDist"; };

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
  double mean_;    /**< Distribution mean */
  double std_dev_; /**< Distribution standard deviation */
  boost::math::inverse_gaussian distribution_; /**< Inverse Gaussian
                                                  distribution */
};
}  // namespace stochastic

#endif  // _INV_GAUSS_DIST_H_
