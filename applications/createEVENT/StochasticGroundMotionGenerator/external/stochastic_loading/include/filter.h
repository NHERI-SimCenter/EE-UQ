#ifndef _FILTER_H_
#define _FILTER_H_

#include <memory>
#include <stdexcept>
#include <vector>
#include <intel_ipp/include/ipps.h>

/**
 * Signal processing functionality
 */
namespace signal_processing {

/**
 * Function for calculating the coefficients of the highpass Butterworth filter
 * @param[in] filter_order Order of the Butterworth filter
 * @param[in] cuttoff_freq Normalized cutoff frequency
 * @return Returns a vector containing two vectors where the first vector
 *         contains and numerator coefficients and the second vector contains
 *         the denominator coefficients.
 */
std::function<std::vector<std::vector<double>>(int, double)>
    hp_butterworth =
        [](int filter_order,
           double cutoff_freq) -> std::vector<std::vector<double>> {
  // Allocated memory for coefficients
  std::vector<Ipp64f> taps(2 * (filter_order + 1));
  IppStatus status = ippStsNoErr;
  int internal_buffer_size;

  // Calculate required buffer size for internal calculations
  status = ippsIIRGenGetBufferSize(filter_order, &internal_buffer_size);
  if (status != ippStsNoErr) {
    throw std::runtime_error(
        "\nERROR: in signal_processing::hp_butterworth: Error in buffer size "
        "calculations\n");
  }
  
  // Divide by 2 to make cutoff frequency match the definition given in MATLAB
  Ipp8u * internal_calcs = ippsMalloc_8u(internal_buffer_size);
  status = ippsIIRGenHighpass_64f(cutoff_freq / 2.0, 0, filter_order, taps.data(),
                                  ippButterworth, internal_calcs);

  // Check if filter computation succeeded
  if (status != ippStsNoErr) {
    throw std::runtime_error(
        "\nERROR: in signal_processing::hp_butterworth: Error in coefficient "
        "calculations\n");
  }
  
  std::vector<double> numerator(filter_order + 1);
  std::vector<double> denominator(filter_order + 1);

  for (int i = 0; i < filter_order + 1; ++i) {
    numerator[i] = taps[i];
    denominator[i] = taps[i + filter_order + 1];
  }

  // Free memory associated with internal calcs
  ippsFree(internal_calcs);
  
  return std::vector<std::vector<double>>{numerator, denominator};
};

/**
 * Function that calculates the impulse response of a filter defined by the
 * input numerator and denominator coefficients for the input number of samples
 * @param[in] numerator_coeffs Numerator coefficients for filter
 * @param[in] denominator_coeffs Denominator coefficients for filter
 * @param[in] order Order of the filter
 * @param[in] num_samples Number of samples desired
 * @return Vector containing impulse response for requested number of samples
 */
std::function<std::vector<double>(std::vector<double>, std::vector<double>,
                                  int, int)>
    impulse_response = [](const std::vector<double>& numerator_coeffs,
                          const std::vector<double>& denominator_coeffs,
                          int order, int num_samples) -> std::vector<double> {
  if (numerator_coeffs.size() != denominator_coeffs.size()) {
    throw std::runtime_error(
        "\nERROR: in signal_processing::impulse_response: Inputs for numerator "
        "and denominator coefficients not same length\n");
  }
  
  IppStatus status = ippStsNoErr;
  int internal_buffer_size;
  IppsIIRState_64f * filter_state = nullptr;
  Ipp64f *samples = ippsMalloc_64f(num_samples),
         *impulse = ippsMalloc_64f(num_samples);
  std::vector<double> taps(numerator_coeffs.size() + denominator_coeffs.size());

  // Set all values to zero except first one for impulse
  impulse[0] = 1.0;
  for (int i = 1; i < num_samples; ++i) {
    impulse[i] = 0.0;
  }
  
  // Put filter coefficients into single stack array
  for (unsigned int i = 0; i < numerator_coeffs.size(); ++i) {
    taps[i] = numerator_coeffs[i];
    taps[i + numerator_coeffs.size()] = denominator_coeffs[i];
  }

  // Get buffer size required for internal calcs
  status = ippsIIRGetStateSize_64f(order, &internal_buffer_size);
  if (status != ippStsNoErr) {
    throw std::runtime_error(
        "\nERROR: in signal_processing::impulse_response: Error in buffer size "
        "calculations\n");
  }

  // Allocate memory for internal calcs
  Ipp8u * internal_calcs = ippsMalloc_8u(internal_buffer_size);

  // Initialize filter state
  status = ippsIIRInit_64f(&filter_state, taps.data(), order, nullptr, internal_calcs);
  if (status != ippStsNoErr) {
    throw std::runtime_error(
        "\nERROR: in signal_processing::impulse_response: Error in filter initialization\n");
  }

  // Apply filter to impulse
  status = ippsIIR_64f(impulse, samples, num_samples, filter_state);
  if (status != ippStsNoErr) {
    throw std::runtime_error(
        "\nERROR: in signal_processing::impulse_response: Error in filter application\n");
  }

  std::vector<double> sample_vec(num_samples);
  for (int i = 0; i < num_samples; ++i) {
    sample_vec[i] = samples[i];
  }

  // Free memory used for filtering
  ippsFree(samples);
  ippsFree(impulse);
  ippsFree(internal_calcs);

  return sample_vec;
};
}  // namespace signal_processing

#endif  // _FILTER_H_
