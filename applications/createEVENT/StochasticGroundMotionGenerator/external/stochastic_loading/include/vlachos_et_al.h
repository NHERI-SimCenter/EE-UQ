#ifndef _VLACHOS_ET_AL_H_
#define _VLACHOS_ET_AL_H_

#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include "distribution.h"
#include "json_object.h"
#include "numeric_utils.h"
#include "stochastic_model.h"

namespace stochastic {
/**
 * Stochastic model for generating scenario specific ground
 * motion time histories. This is based on the paper:
 * Vlachos C., Papakonstantinou K.G., & Deodatis G. (2018).
 * Predictive model for site specific simulation of ground motions based on
 * earthquake scenarios. Earthquake Engineering & Structural Dynamics, 47(1),
 * 195-218.
 */
class VlachosEtAl : public StochasticModel {
 public:
  /**
   * @constructor Delete default constructor
   */
  VlachosEtAl() = default;

  /**
   * @constructor Construct scenario specific ground motion model based on input
   * parameters
   * @param[in] moment_magnitude Moment magnitude of earthquake scenario
   * @param[in] rupture_distance Closest-to-site rupture distance in kilometers
   * @param[in] vs30 Soil shear wave velocity averaged over top 30 meters in
   *                 meters per second
   * @param[in] orientation Orientation of acceleration relative to global
   *                        coordinates. Represents counter-clockwise angle (in
   *                        degrees) away from x-axis rotating around z-axis in
   *                        right-handed coordinate system.
   * @param[in] num_spectra Number of evolutionary power spectra that should be
   *                        generated.
   * @param[in] num_sims Number of simulated ground motion time histories that
   *                     should be generated per evolutionary power
   */
  VlachosEtAl(double moment_magnitude, double rupture_distance, double vs30,
              double orientation, unsigned int num_spectra,
              unsigned int num_sims);

  /**
   * @constructor Construct scenario specific ground motion model based on input
   * parameters
   * @param[in] moment_magnitude Moment magnitude of earthquake scenario
   * @param[in] rupture_distance Closest-to-site rupture distance in kilometers
   * @param[in] vs30 Soil shear wave velocity averaged over top 30 meters in
   *                 meters per second
   * @param[in] orientation Orientation of acceleration relative to global
   *                        coordinates. Represents counter-clockwise angle (in
   *                        degrees) away from x-axis rotating around z-axis in
   *                        right-handed coordinate system.
   * @param[in] num_spectra Number of evolutionary power spectra that should be
   *                        generated.
   * @param[in] num_sims Number of simulated ground motion time histories that
   *                     should be generated per evolutionary power
   * @param[in] seed_value Value to seed random variables with to ensure
   *                       repeatability
   */
  VlachosEtAl(double moment_magnitude, double rupture_distance, double vs30,
              double orientation, unsigned int num_spectra,
              unsigned int num_sims, int seed_value);  

  /**
   * @destructor Virtual destructor
   */
  virtual ~VlachosEtAl() {};

  /**
   * Delete copy constructor
   */
  VlachosEtAl(const VlachosEtAl&) = delete;

  /**
   * Delete assignment operator
   */
  VlachosEtAl& operator=(const VlachosEtAl&) = delete;

  /**
   * Generate ground motion time histories based on input parameters
   * and store outputs as JSON object. Throws exception if errors
   * are encountered during time history generation.
   * @param[in] event_name Name to assign to event
   * @param[in] g_units Indicates that time histories should be returned in
   *                    units of g. Defaults to false where time histories
   *                    are returned in units of m/s^2
   * @return JsonObject containing time histories
   */
  utilities::JsonObject generate(const std::string& event_name,
                                 bool g_units = false) override;

  /**
   * Generate ground motion time histories based on input parameters
   * and write results to file in JSON format. Throws exception if
   * errors are encountered during time history generation.
   * @param[in] event_name Name to assign to event
   * @param[in, out] output_location Location to write outputs to
   * @param[in] g_units Indicates that time histories should be returned in
   *                    units of g. Defaults to false where time histories
   *                    are returned in units of m/s^2
   * @return Returns true if successful, false otherwise
   */
  bool generate(const std::string& event_name,
                const std::string& output_location,
                bool g_units = false) override;

  /**
   * Compute a family of time histories for a particular power spectrum
   * @param[in, out] time_histories Location where time histories should be
   *                                stored
   * @param[in] parameters Set of model parameters to use for calculating power
   *                       specturm and time histories
   * @return Returns true if successful, false otherwise
   */
  bool time_history_family(std::vector<std::vector<double>>& time_histories,
                           const Eigen::VectorXd& parameters) const;

  /**
   * Simulate fully non-stationary ground motion sample realization based on
   * time and frequency discretization and the discretized evolutionary
   * power spectrum. This is described by Eq-19 on page 8.
   * @param[in, out] time_history Location where time history should be stored
   * @param[in] power_spectrum Matrix containing values of power spectrum over
   *                           range of frequencies at specified times.
   */
  void simulate_time_history(std::vector<double>& time_history,
                             const Eigen::MatrixXd& power_spectrum) const;

  /**
   * Post-process the input time history as described in Vlachos et al. using
   * multiple-window estimation technique after Conte & Peng (1997) and
   * highpass Butterworth filter
   * @param[in, out] time_history Time history to post-process. Post-processed
   *                              results are also stored here.
   * @param[in] filter_imp_resp Impulse response of Butterworth filter
   * @return Returns true if successful, false otherwise
   */
  bool post_process(std::vector<double>& time_history,
                    const std::vector<double>& filter_imp_resp) const;

  /**
   * Identifies modal frequency parameters for mode 1 and 2
   * @param[in] initial_params Initial set of parameters
   * @return Vector of identified parameters
   */
  Eigen::VectorXd identify_parameters(const Eigen::VectorXd& initial_params) const;

  /**
   * Calculate the dominant modal frequencies as a function of non-dimensional
   * cumulative energy and the model parameters Q_k, alpha_k, and beta_k where
   * k is the mode (either 1 or 2). This is defined by Eq-8 on page 6.
   * @param[in] parameters Vector of values for alpha_k, beta_k and Q_k at k-th
   *                       mode
   * @param[in] energy Vector of non-dimensional energy values at which to
   *                   calculate frequency
   * @return Vector of frequency values at input energy values
   */
  std::vector<double> modal_frequencies(
      const std::vector<double>& parameters,
      const std::vector<double>& energy) const;

  /**
   * Calculate the energy accumulation over the times specified in the inputs as
   * defined by Eq-5 on page 6.
   * @param[in] parameters Vector of values containing energy parameters gamma
   *                       and delta
   * @param[in] times Vector containing non-dimensional times at which to
   *                  calculate energy accumulation
   * @return Vector containing accumulated energy values at specified input
   *         times
   */
  std::vector<double> energy_accumulation(
      const std::vector<double>& parameters,
      const std::vector<double>& times) const;

  /**
   * Calculate the logarithmic normalized modal participation factor for the
   * second mode as defined by Eq-11 on page 7.
   * @param[in] parameters Vector of values for F(I), mu(I), sigma(I), F(II),
   *                       mu(II) and sigma(II)
   * @param[in] energy Vector of non-dimensional energy values at which to
   *                   calculate frequency
   * @return Vector containing values of logarithmic normalized modal
   *         participation factor for second mode at input energy values
   */
  std::vector<double> modal_participation_factor(
      const std::vector<double>& parameters,
      const std::vector<double>& energy) const;

  /**
   * Calculate amplitude modulating function as defined by Eq-7 on page 6
   * @param[in] duration Total duration of target seismic record
   * @param[in] total_energy Total energy content of seismic ground acceleration
   * @param[in] parameters Vector of values containing energy parameters gamma
   *                       and delta
   * @param[in] times Vector containing non-dimensional times at which to
   *                  calculate energy accumulation
   * @return Vector containing values of amplitude modulating function at input
   *         times
   */
  std::vector<double> amplitude_modulating_function(
      double duration, double total_energy,
      const std::vector<double>& parameters,
      const std::vector<double>& times) const;

  /**
   * Calculate evolutionary power spectrum at specific time using parametric,
   * bimodal, fully non-stationary Kinai-Tajimi (K-T) model of seismic ground
   * acceleration signal consisting of 2 distinct spectral modes. This is
   * described by Eq-1 on page 4.
   * @param[in] parameters Vector of parameters fg_1, zeta_1, S0_1, fg_2, zeta_2
   *                       and S0_2 which define the time varying modal dominant
   *                       frequency, model apparent damping ratio, and modal
   *                       participation factor associated with the two modes.
   *                       NOTE: Values of fg_1, fg_2 and S0_2 must be for the
   *                       particular time at which the power spectrum is
   *                       desired
   * @param[in] frequencies Vector of frequencies at which to evaluate
   *                        evolutionary power spectrum
   * @param[in] highpass_butter Vector of Butterworth filter transfer function
   *                            energy content at input frequencies
   * @return Vector containing values for model evolutionary power spectrum at
   *         time defined by input parameters fg_1, fg_2 and S0_2.
   */
  Eigen::VectorXd kt_2(const std::vector<double>& parameters,
                       const std::vector<double>& frequencies,
                       const std::vector<double>& highpass_butter) const;

  /**
   * Rotate acceleration based on orientation angle
   * @param[in] acceleration Acceleration to rotate
   * @param[in, out] x_accels Vector to store x-component of acceleration to
   * @param[in, out] y_accels Vector to story y-component of acceleration to
   * @param[in] g_units Indicates that time histories should be returned in
   *                    units of g
   */
  void rotate_acceleration(const std::vector<double>& acceleration,
                           std::vector<double>& x_accels,
                           std::vector<double>& y_accels, bool g_units) const;

 private:
  double moment_magnitude_; /**< Moment magnitude for scenario */
  double rupture_dist_; /**< Closest-to-site rupture distance in kilometers */
  double vs30_; /**< Soil shear wave velocity averaged over top 30 meters in
                   meters per second */
  double orientation_; /**< Counter-clockwise angle away from global x-axis */
  double time_step_; /**< Temporal discretization. Set to 0.01 seconds */
  double freq_step_; /**< Frequency discretization. Set to 0.2 Hz */
  double cutoff_freq_; /**< Cutoff frequency */
  unsigned int num_spectra_; /**< Number of evolutionary power spectra that
                                should be generated */
  unsigned int num_sims_; /**< Number of simulated ground motion time histories
                             that should be generated per evolutionary power
                             spectrum */
  int seed_value_; /**< Integer to seed random distributions with */
  Eigen::VectorXd means_; /**< Mean values of model parameters */
  Eigen::MatrixXd covariance_; /**< Covariance matrix for model parameters */
  std::vector<std::shared_ptr<stochastic::Distribution>>
      model_parameters_; /**< Distrubutions for 18-parameter model */
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
      parameter_realizations_; /**< Random realizations of normal model parameters */
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
      physical_parameters_; /**< Normal parameters transformed to
                               physical space */
  std::shared_ptr<numeric_utils::RandomGenerator>
      sample_generator_; /**< Multivariate normal random number generator */
};
}  // namespace stochastic

#endif  // _VLACHOS_ET_AL_H_
