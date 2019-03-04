#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <cmath>
#include <functional>
#include <tuple>
#include <vector>
#include <Eigen/Dense>

/**
 * Signal processing functionality
 */
namespace signal_processing {

/**
 * Function that calculated the Hann window for the input window length
 * @param[in] window_length Desired length of window
 * @return Vector filled with Hann window function evaluations based on input
 *         window length
 */
std::function<Eigen::VectorXd(unsigned int)> hann_window =
    [](unsigned int window_length) -> Eigen::VectorXd {
  Eigen::VectorXd hann(window_length);
  double number_of_points = static_cast<double>(window_length - 1);

  for (unsigned int i = 0; i < hann.size(); ++i) {
    hann[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / number_of_points));
  }

  return hann;
};
}  // namespace signal_processing

#endif  // _WINDOW_H_
