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
#include <RandomVariableInputWidget.h>
#include "StochasticModelWidget.h"
#include "StochasticMotionInputWidget.h"
#include "VlachosEtAlModel.h"


StochasticMotionInputWidget::StochasticMotionInputWidget(
    RandomVariableInputWidget* random_variables, QWidget* parent)
    : SimCenterAppWidget(parent),
      rv_input_widget_(random_variables) {
  // Construct required layouts
  QVBoxLayout* layout = new QVBoxLayout();
  QHBoxLayout* model_layout = new QHBoxLayout();
  QHBoxLayout* parameters_layout = new QHBoxLayout();
  
  // Create label and add items to combo box for model selection 
  QLabel* selection_label = new QLabel(tr("Stochastic Loading Model"));
  model_selection_ = new QComboBox();
  model_selection_->addItem(tr("Vlachos et al. (2018)"));
  stochastic_model_ = new VlachosEtAlModel(rv_input_widget_, this);
  
  // Add widgets to layouts and layouts to this
  model_layout->addWidget(model_selection_);
  model_layout->addStretch();
  parameters_layout->addWidget(stochastic_model_);
  parameters_layout->addStretch();
  layout->addLayout(model_layout);
  layout->addLayout(parameters_layout);
  layout->addStretch();
  this->setLayout(layout);

  // Connect model selection slot
  connect(model_selection_,
          QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this,
          &StochasticMotionInputWidget::modelSelectionChanged);
}


StochasticMotionInputWidget::~StochasticMotionInputWidget()
{}

bool StochasticMotionInputWidget::outputToJSON(QJsonObject& jsonObject) {
  bool result = true;
  // jsonObject["EventClassification"] = "Earthquake";
  // jsonObject["type"] = "StochasticMotion";
  // jsonObject["modelName"] = model_selection_->currentText();

  // if (model_selection_->currentText() == "Vlachos et al. (2018)") {
  //   jsonObject["momentMagnitude"] =
  //       static_cast<QDoubleSpinBox*>(
  //           (model_inputs_->itemAt(0, QFormLayout::FieldRole))->widget())
  //           ->value();
  //   jsonObject["ruptureDist"] =
  //       static_cast<QDoubleSpinBox*>(
  //           (model_inputs_->itemAt(1, QFormLayout::FieldRole))->widget())
  //           ->value();
  //   jsonObject["vs30"] =
  //       static_cast<QDoubleSpinBox*>(
  //           (model_inputs_->itemAt(2, QFormLayout::FieldRole))->widget())
  //           ->value();
  //   jsonObject["numberOfEvents"] =
  //       static_cast<QSpinBox*>(
  //           (model_inputs_->itemAt(3, QFormLayout::FieldRole))->widget())
  //           ->value();
  // } else {
  //   result = false;
  // }

  return result;
}

bool StochasticMotionInputWidget::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;
  
  // if (jsonObject.value("type").toString() == "StochasticMotion") {
  //   auto app_data = jsonObject;
  //   if (app_data["modelName"].toString() == "Vlachos et al. (2018)") {
  //     model_selection_->setCurrentText(app_data["modelName"].toString());
  //     // Delete previous entries
  //     while (model_inputs_->rowCount() > 0) {
  //       model_inputs_->removeRow(0);
  //     }
  //     QDoubleSpinBox* moment_mag = new QDoubleSpinBox();
  //     QDoubleSpinBox* rupt_dist = new QDoubleSpinBox();
  //     QDoubleSpinBox* vs30 = new QDoubleSpinBox();
  //     QSpinBox* num_events = new QSpinBox();
  //     moment_mag->setRange(0.0, std::numeric_limits<double>::infinity());
  //     rupt_dist->setRange(0.0, std::numeric_limits<double>::infinity());
  //     vs30->setRange(0.0, std::numeric_limits<double>::infinity());
  //     num_events->setRange(1, 900000);

  //     moment_mag->setValue(app_data["momentMagnitude"].toDouble());
  //     rupt_dist->setValue(app_data["ruptureDist"].toDouble());
  //     vs30->setValue(app_data["vs30"].toDouble());
  //     num_events->setValue(app_data["numberOfEvents"].toInt());

  //     model_inputs_->addRow(new QLabel(tr("Moment Magnitude")), moment_mag);
  //     model_inputs_->addRow(
  //         new QLabel(tr("Closest-to-Site Rupture Distance [km]")), rupt_dist);
  //     model_inputs_->addRow(
  //         new QLabel(tr("Average shear-wave velocity for top 30 meters [m/s]")),
  //         vs30);
  //     model_inputs_->addRow(new QLabel(tr("Number of events to generate")),
  //                           num_events);
  //   }
  // } else {
  //   result = false;
  // }

  return result;
}

bool StochasticMotionInputWidget::outputAppDataToJSON(QJsonObject& jsonObject) {
  bool result = true;

  // jsonObject["Application"] = "StochasticGroundMotion";
  // QJsonObject app_data;

  // if (model_selection_->currentText() == "Vlachos et al. (2018)") {
  //   app_data.insert("modelName", "vlachos2018");
  //   app_data.insert(
  //       "momentMagnitude",
  //       static_cast<QDoubleSpinBox*>(
  //           (model_inputs_->itemAt(0, QFormLayout::FieldRole))->widget())
  //           ->value());
  //   app_data.insert(
  //       "ruptureDist",
  //       static_cast<QDoubleSpinBox*>(
  //           (model_inputs_->itemAt(1, QFormLayout::FieldRole))->widget())
  //           ->value());
  //   app_data.insert(
  //       "vs30",
  //       static_cast<QDoubleSpinBox*>(
  //           (model_inputs_->itemAt(2, QFormLayout::FieldRole))->widget())
  //           ->value());
  //   app_data.insert(
  //       "numberOfEvents",
  //       static_cast<QSpinBox*>(
  //           (model_inputs_->itemAt(3, QFormLayout::FieldRole))->widget())
  //           ->value());
  //   jsonObject["ApplicationData"] = app_data;
  //   jsonObject["EventClassification"] = "Earthquake";
  // } else {
  //   result = false;
  // }

  return result;
}

bool StochasticMotionInputWidget::inputAppDataFromJSON(QJsonObject& jsonObject) {
  return true;
}

void StochasticMotionInputWidget::modelSelectionChanged(const QString& model) {
  // Switch the model description and form layout based on model selection
  if (model == "Vlachos et al. (2018)") {
    stochastic_model_ = new VlachosEtAlModel(rv_input_widget_, this);
  } else {
    qDebug() << "ERROR: In StochasticMotionInputWidget::modelSelectionChanged: "
                "Unknown selection: "
             << model << "\n";
  }
}

void StochasticMotionInputWidget::errorMessage(QString message) {
  emit sendErrorMessage(message);
}
