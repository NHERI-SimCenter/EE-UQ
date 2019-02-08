#ifndef _STUDENTS_T_DIST_H_
#define _STUDENTS_T_DIST_H_

#include <string>
#include <vector>
#include <boost/math/distributions/students_t.hpp>
#include "distribution.h"

namespace stochastic {
/**
 * Student's t distribution
 */
class StudentstDistribution : public Distribution {
 public:
  /**
   * @constructor Delete default constructor
   */
  StudentstDistribution() = delete;

  /**
   * @constructor Construct Student's t distribution with specified mean,
   * standard deviation and degrees of freedom
   * @param[in] mean Mean of distribution
   * @param[in] std_dev Standard deviation of distribution
   * @param[in] dof Degrees of freedom of distribution
   */
  StudentstDistribution(double mean, double scale, double dof);

  /**
   * @destructor Virtual destructor
   */
  virtual ~StudentstDistribution(){};

  /**
   * Get the name of the distribution model
   * @return Model name as a string
   */
  std::string name() const override { return "StudentstDist"; };

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
  double mean_;                          /**< Distribution mean */
  double std_dev_;                       /**< Distribution standard deviation */
  double dof_;                           /**< Degrees of freedom */
  boost::math::students_t distribution_; /**< Student's t distribution */
};
}  // namespace stochastic

#endif  // _STUDENTS_T_DIST_H_
