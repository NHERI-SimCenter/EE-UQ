#ifndef _BETA_DIST_H_
#define _BETA_DIST_H_

#include <string>
#include <vector>
#include <boost/math/distributions/beta.hpp>
#include "distribution.h"

namespace stochastic {
/**
 * Beta distribution
 */
class BetaDistribution : public Distribution {
 public:
  /**
   * @constructor Delete default constructor
   */
  BetaDistribution() = delete;

  /**
   * @constructor Construct beta distribution with specified mean and
   * standard deviation
   * @param[in] alpha Shape parameter
   * @param[in] beta Shape parameter
   */
  BetaDistribution(double alpha, double beta);

  /**
   * @destructor Virtual destructor
   */
  virtual ~BetaDistribution(){};

  /**
   * Get the name of the distribution model
   * @return Model name as a string
   */
  std::string name() const override { return "BetaDist"; };

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
  double alpha_;    /**< Shape parameter */
  double beta_; /**< Shape parameter */
  boost::math::beta_distribution<double>
    distribution_; /**< Beta distribution */
};
}  // namespace stochastic

#endif  // _BETA_DIST_H_
