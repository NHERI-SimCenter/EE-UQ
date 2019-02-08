#ifndef _NUMERIC_UTILS_H_
#define _NUMERIC_UTILS_H_

#include <ctime>
#include <utility>
#include <vector>
#include <Eigen/Dense>

/**
 * Numeric utility functions not tied to any particular class
 */
namespace numeric_utils {

/**
 * Convert input correlation matrix and standard deviation to covariance matrix
 * @param[in] corr Input correlation matrix
 * @param[in] std_dev Standard deviation vector
 * @return Covariance matrix with same dimensions as input correlation matrix
 */
Eigen::MatrixXd corr_to_cov(const Eigen::MatrixXd& corr,
			    const Eigen::VectorXd& std_dev);

/**
 * Compute the 1-dimensional convolution of two input vectors
 * @param[in] input_x First input vector of data
 * @param[in] input_y Second input vector of data
 * @param[out] output Vector to story convolution results to
 * @return Returns true if convolution was successful, false otherwise
 */
bool convolve_1d(const std::vector<double>& input_x,
                 const std::vector<double>& input_y,
                 std::vector<double>& response);

/**
 * Calculate the integral of the input vector with uniform spacing
 * between data points
 * @param[in] input_vector Vector containing function values
 * @param[in] spacing Spacing between data points
 * @return Approximate value of function integral
 */
double trapazoid_rule(const std::vector<double>& input_vector, double spacing);

/**
 * Calculate the integral of the input vector with uniform spacing
 * between data points
 * @param[in] input_vector Vector containing function values
 * @param[in] spacing Spacing between data points
 * @return Approximate value of function integral
 */
double trapazoid_rule(const Eigen::VectorXd& input_vector, double spacing);

/**
 * Abstract base class for random number generators
 */
class RandomGenerator {
 public:
  /**
   * @constructor Default constructor
   */
  RandomGenerator() = default;

  /**
   * @destructor Virtual destructor
   */
  virtual ~RandomGenerator() {};

  /**
   * Delete copy constructor
   */
  RandomGenerator(const RandomGenerator&) = delete;

  /**
   * Delete assignment operator
   */
  RandomGenerator& operator=(const RandomGenerator&) = delete;

  /**
   * Get multivariate random realization
   * @param[in, out] random_numbers Matrix to store generated random numbers to
   * @param[in] means Vector of mean values for random variables
   * @param[in] cov Covariance matrix of for random variables
   * @param[in] cases Number of cases to generate
   * @return Returns true if no issues were encountered in Cholesky
   *         decomposition of covariance matrix, returns false otherwise
   */
  virtual bool generate(
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& random_numbers,
      const Eigen::VectorXd& means, const Eigen::MatrixXd& cov,
      unsigned int cases = 1) = 0;

  /**
   * Get the class name
   * @return Class name
   */
   virtual std::string name() const = 0;
  
 protected:
  int seed_ = static_cast<int>(
      std::time(nullptr)); /**< Seed value to use in random number generator */
};
}  // namespace numeric_utils

#endif  // _NUMERIC_UTILS_H_
