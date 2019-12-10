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
#include <QDebug>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <RandomVariablesContainer.h>
#include "StochasticModelWidget.h"
#include "StochasticMotionInput.h"
#include "VlachosEtAlModel.h"
#include "DabaghiDerKiureghianPulse.h"

StochasticMotionInput::StochasticMotionInput(
    RandomVariablesContainer* random_variables, QWidget* parent)
    : SimCenterAppWidget(parent),
      rv_input_widget_(random_variables) {
  // Construct required layouts
  QVBoxLayout* layout = new QVBoxLayout();
  QHBoxLayout* model_layout = new QHBoxLayout();
  parameters_layout_ = new QHBoxLayout();
  
  // Create label and add items to combo box for model selection 
  QLabel* selection_label = new QLabel(tr("Stochastic Loading Model"));
  //selection_label->setStyleSheet("font-weight: bold");
  model_selection_ = new QComboBox();
  model_selection_->setObjectName("StochasticLoadingModel");
  model_selection_->addItem(tr("Vlachos et al. (2018)"));
  model_selection_->addItem(tr("Dabaghi & Der Kiureghian (2018)"));  
  stochastic_model_ = new VlachosEtAlModel(rv_input_widget_, this);
  
  // Add widgets to layouts and layouts to this
  model_layout->addWidget(model_selection_);
  model_layout->addStretch();
  parameters_layout_->addWidget(stochastic_model_);
  parameters_layout_->addStretch();
  layout->addWidget(selection_label);
  layout->addLayout(model_layout);
  layout->addLayout(parameters_layout_);
  layout->addStretch();
  this->setLayout(layout);

  // Connect model selection slot
  connect(model_selection_,
          QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this,
          &StochasticMotionInput::modelSelectionChanged);
}


StochasticMotionInput::~StochasticMotionInput()
{}

bool StochasticMotionInput::outputToJSON(QJsonObject& jsonObject) {
  bool result = true;
  jsonObject["EventClassification"] = "Earthquake";
  jsonObject["type"] = "StochasticMotion";
  jsonObject["modelName"] = model_selection_->currentText();

  QJsonObject model_data;
  stochastic_model_->outputToJSON(model_data);
  
  foreach (const QString& key, model_data.keys()) {
    jsonObject.insert(key, model_data.value(key));
  }

  return result;
}

bool StochasticMotionInput::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;

  if (jsonObject.value("type").toString() == "StochasticMotion") {
    auto app_data = jsonObject;
    if (app_data["modelName"].toString() == "Vlachos et al. (2018)") {
      model_selection_->setCurrentText(app_data["modelName"].toString());
      // Assign current model to temporary pointer and create new model
      auto temp_model = stochastic_model_;
      stochastic_model_ = new VlachosEtAlModel(rv_input_widget_, this);
      stochastic_model_->inputFromJSON(jsonObject);
      // Replace current widget with new one and delete current one
      parameters_layout_->replaceWidget(temp_model, stochastic_model_);
      parameters_layout_->addStretch();
      delete temp_model;
      temp_model = nullptr;
    } else if (app_data["modelName"].toString() ==
               "Dabaghi & Der Kiureghian (2018)") {
      model_selection_->setCurrentText(app_data["modelName"].toString());      
      // Assign current model to temporary pointer and create new model
      auto temp_model = stochastic_model_;
      stochastic_model_ = new DabaghiDerKiureghianPulse(rv_input_widget_, this);
      stochastic_model_->inputFromJSON(jsonObject);
      // Replace current widget with new one and delete current one
      parameters_layout_->replaceWidget(temp_model, stochastic_model_);
      parameters_layout_->addStretch();
      delete temp_model;
      temp_model = nullptr;
    }
  } else {
    result = false;
  }

  return result;
}

bool StochasticMotionInput::outputAppDataToJSON(QJsonObject& jsonObject) {
  bool result = true;

  jsonObject["Application"] = "StochasticGroundMotion";
  QJsonObject app_data;
  QJsonObject model_data;
  stochastic_model_->outputToJSON(model_data);  

  if (model_selection_->currentText() == "Vlachos et al. (2018)") {
    app_data.insert("modelName", "VlachosSiteSpecificEQ");
    app_data.insert("seed", model_data.value("seed"));

    jsonObject["ApplicationData"] = app_data;
    jsonObject["EventClassification"] = "Earthquake";
  } else if (model_selection_->currentText() ==
             "Dabaghi & Der Kiureghian (2018)") {
    app_data.insert("modelName", "DabaghiDerKiureghianNFGM");
    app_data.insert("seed", model_data.value("seed"));

    jsonObject["ApplicationData"] = app_data;
    jsonObject["EventClassification"] = "Earthquake";    
  } else {
    result = false;
  }

  return result;
}

bool StochasticMotionInput::inputAppDataFromJSON(QJsonObject& jsonObject) {
  return true;
}

void StochasticMotionInput::modelSelectionChanged(const QString& model) {
  // Switch the model description and form layout based on model selection
  if (model == "Vlachos et al. (2018)") {
    auto temp_model = stochastic_model_;
    stochastic_model_ = new VlachosEtAlModel(rv_input_widget_, this);
    // Replace current widget with new one and delete current one
    parameters_layout_->replaceWidget(temp_model, stochastic_model_);
    parameters_layout_->addStretch();
    delete temp_model;
    temp_model = nullptr;
  } else if (model == "Dabaghi & Der Kiureghian (2018)") {
    auto temp_model = stochastic_model_;
    stochastic_model_ = new DabaghiDerKiureghianPulse(rv_input_widget_, this);
    // Replace current widget with new one and delete current one
    parameters_layout_->replaceWidget(temp_model, stochastic_model_);
    parameters_layout_->addStretch();
    delete temp_model;
    temp_model = nullptr;
  } else {
    qDebug() << "ERROR: In StochasticMotionInput::modelSelectionChanged: "
                "Unknown selection: "
             << model << "\n";
  }
}

void StochasticMotionInput::errorMessage(QString message) {
  emit sendErrorMessage(message);
}
