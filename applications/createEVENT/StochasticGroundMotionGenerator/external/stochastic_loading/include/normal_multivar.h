#ifndef _NORMAL_MULTIVAR_H_
#define _NORMAL_MULTIVAR_H_

// Boost random generator
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
// Eigen dense matrices
#include <Eigen/Dense>

#include "numeric_utils.h"

namespace numeric_utils {
/**
 * Class for generating random realizations of a multivariate
 * normal distribution
 */
class NormalMultiVar : public RandomGenerator {
 public:
  /**
   * @constructor Default constructor
   */
  NormalMultiVar();

  /**
   * @constructor Construct an instance of the multivariate normal random number
   * generator
   * @param[in] seed Seed value to use in random number generator
   */
  NormalMultiVar(int seed);

  /**
   * @destructor Virtual destructor
   */
  virtual ~NormalMultiVar(){};

  /**
   * Get multivariate random realization
   * @param[in, out] random_numbers Matrix to store generated random numbers to
   * @param[in] means Vector of mean values for random variables
   * @param[in] cov Covariance matrix of for random variables
   * @param[in] cases Number of cases to generate
   * @return Returns true if no issues were encountered in Cholesky
   *         decomposition of covariance matrix, returns false otherwise
   */
  bool generate(
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& random_numbers,
      const Eigen::VectorXd& means, const Eigen::MatrixXd& cov,
      unsigned int cases = 1) override;

  /**
   * Get the class name
   * @return Class name
   */  
  std::string name() const override;

 private:
  boost::random::mt19937 generator_; /**< Mersenne Twister random number
                                        generator */
  boost::random::normal_distribution<double> distribution_; /**< Normal
                                                         distribution to use
                                                         with random number
                                                         generator */
};
}  // namespace numeric_utils

#endif  // _NORMAL_MULTIVAR_H_
