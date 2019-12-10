#ifndef _STOCHASTIC_MOTION_INPUT_H
#define _STOCHASTIC_MOTION_INPUT_H

/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: mhgardner

#include <QComboBox>
#include <QJsonObject>
#include <QString>
#include <QWidget>
#include <RandomVariablesContainer.h>
#include <SimCenterAppWidget.h>
#include "StochasticModelWidget.h"

// Forward declarations
class QComboBox;
class QJsonObject;
class QString;
class QWidget;
class RandomVariableInputWidget;

/**
 * Widget for inputting parameters for stochastic earthquake time history
 * generation
 */
class StochasticMotionInput : public SimCenterAppWidget {
  Q_OBJECT
 public:
  /**
   * @constructor Construct new stochastic motion input widget
   * @param[in, out] random_variables Widget to store random variables to
   * @param[in, out] parent Pointer to parent widget. Defaults to nullptr.
   */
  explicit StochasticMotionInput(RandomVariablesContainer* random_variables,
				       QWidget* parent = nullptr);

  /**
   * @destructor Virtual desctructor for stochastic input widget
   */
  virtual ~StochasticMotionInput();

  /**
   * Instantiate stochastice motion input widger from input JSON object
   * @param[in] rvObject JSON object containing input information
   * @return Returns true if successful, false otherwise
   */
  bool inputFromJSON(QJsonObject& rvObject);

  /**
   * Write all current class data to JSON required to reconstruct class
   * @param[in, out] rvObject JSON object to write output to
   * @return Returns true if successful, false otherwise
   */
  bool outputToJSON(QJsonObject& rvObject) override;

  /**
   * Read application-specific data from JSON object
   * @param[in] rvObject JSON object to read application data from
   * @return Returns true if successful, false otherwise
   */
  bool inputAppDataFromJSON(QJsonObject& rvObject) override;

  /**
   * Write application-specific data to JSON object
   * @param[in, out] rvObject JSON object to write application data to
   * @return Returns true if successful, false otherwise
   */
  bool outputAppDataToJSON(QJsonObject& rvObject) override;

 signals:
  void sendErrorMessage(QString message);

 public slots:
  /**
   * Emit error message displaing input message
   * @param[in] message String containg error message to emit
   */
  void errorMessage(QString message);

  /**
   * Emit change in model selection
   * @param[in] model String name currently selected model
   */
  void modelSelectionChanged(const QString& model);

 private:
  RandomVariablesContainer* rv_input_widget_; /**< Widget for inputting random
						  variables */
  QHBoxLayout * parameters_layout_; /**< Layout for stochastic model widget */
  QComboBox* model_selection_; /**< Selection of ground motion model inputs */
  StochasticModelWidget* stochastic_model_; /**< Widget for inputting currently
					       selected model parameters */
};

#endif  // _STOCHASTIC_MOTION_INPUT_H
