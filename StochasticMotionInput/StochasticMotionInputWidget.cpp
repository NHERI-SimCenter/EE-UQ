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

#include "StochasticMotionInput.h"

StochasticMotionInput::StochasticMotionInput(
    RandomVariableInputWidget* random_variables, QWidget* parent)
    : SimCenterAppWidget(parent),
      rv_input_widget(random_variables) {
  // Construct required layouts
  QVBoxLayout* layout = new QVBoxLayout();
  QHBoxLayout* selection_layout = new QHBoxLayout();
  QHBoxLayout* model_layout = new QHBoxLayout();

  // Create label and add items to combo box for model selection 
  QLabel* selection_label = new QLabel(tr("Stochastic Loading Model"));
  model_selection_ = new QComboBox();
  model_selection_->addItem(tr("Vlachos et al. (2018)"));
  model_description_ = new QLabel(
      tr("This model implements the method described in Vlachos et\nal. (2018) "
         "- \"Predictive model for site specific simulation of\nground motions "
         "based on earthquake scenarions\""));
  model_description_->setStyleSheet("QLabel { color : gray; }");  
  model_selection_->addItem(tr("Testing"));

  // Add form layout for model inputs
  model_inputs_ = new QFormLayout();
  QDoubleSpinBox * moment_mag = new QDoubleSpinBox();
  QDoubleSpinBox * rupt_dist = new QDoubleSpinBox();
  QDoubleSpinBox * vs30 = new QDoubleSpinBox();
  QSpinBox * num_events = new QSpinBox();
  moment_mag->setRange(0.0, std::numeric_limits<double>::infinity());
  rupt_dist->setRange(0.0, std::numeric_limits<double>::infinity());
  vs30->setRange(0.0, std::numeric_limits<double>::infinity());
  num_events->setRange(1, 900000);  
  model_inputs_->addRow(new QLabel(tr("Moment Magnitude")), moment_mag);
  model_inputs_->addRow(new QLabel(tr("Closest-to-Site Rupture Distance [km]")),
                        rupt_dist);
  model_inputs_->addRow(
      new QLabel(tr("Average shear-wave velocity for top 30 meters [m/s]")),
      vs30);
  model_inputs_->addRow(new QLabel(tr("Number of events to generate")), num_events);

  // Add widgets to layouts and layouts to this
  selection_layout->addWidget(selection_label);
  selection_layout->addWidget(model_selection_);
  selection_layout->addStretch();
  model_layout->addLayout(model_inputs_);
  model_layout->addStretch();
  
  layout->addLayout(selection_layout);
  layout->addWidget(model_description_);
  layout->addWidget(new QLabel());    
  layout->addLayout(model_layout);
  layout->addStretch();
  this->setLayout(layout);

  // Connect model selection slot
  connect(model_selection_, SIGNAL(currentIndexChanged(QString)), this,
          SLOT(modelSelectionChanged(QString)));
}


StochasticMotionInput::~StochasticMotionInput()
{}

bool StochasticMotionInput::outputToJSON(QJsonObject& jsonObject) {
  bool result = true;
  jsonObject["EventClassification"] = "Earthquake";
  jsonObject["type"] = "StochasticMotion";
  jsonObject["modelName"] = model_selection_->currentText();

  if (model_selection_->currentText() == "Vlachos et al. (2018)") {
    jsonObject["momentMagnitude"] =
        static_cast<QDoubleSpinBox*>(
            (model_inputs_->itemAt(0, QFormLayout::FieldRole))->widget())
            ->value();
    jsonObject["ruptureDist"] =
        static_cast<QDoubleSpinBox*>(
            (model_inputs_->itemAt(1, QFormLayout::FieldRole))->widget())
            ->value();
    jsonObject["vs30"] =
        static_cast<QDoubleSpinBox*>(
            (model_inputs_->itemAt(2, QFormLayout::FieldRole))->widget())
            ->value();
    jsonObject["numberOfEvents"] =
        static_cast<QSpinBox*>(
            (model_inputs_->itemAt(3, QFormLayout::FieldRole))->widget())
            ->value();
  } else {
    result = false;
  }

  return result;
}

bool StochasticMotionInput::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;
  
  if (jsonObject.value("type").toString() == "StochasticMotion") {
    auto app_data = jsonObject;
    if (app_data["modelName"].toString() == "Vlachos et al. (2018)") {
      model_selection_->setCurrentText(app_data["modelName"].toString());
      // Delete previous entries
      while (model_inputs_->rowCount() > 0) {
        model_inputs_->removeRow(0);
      }
      QDoubleSpinBox* moment_mag = new QDoubleSpinBox();
      QDoubleSpinBox* rupt_dist = new QDoubleSpinBox();
      QDoubleSpinBox* vs30 = new QDoubleSpinBox();
      QSpinBox* num_events = new QSpinBox();
      moment_mag->setRange(0.0, std::numeric_limits<double>::infinity());
      rupt_dist->setRange(0.0, std::numeric_limits<double>::infinity());
      vs30->setRange(0.0, std::numeric_limits<double>::infinity());
      num_events->setRange(1, 900000);

      moment_mag->setValue(app_data["momentMagnitude"].toDouble());
      rupt_dist->setValue(app_data["ruptureDist"].toDouble());
      vs30->setValue(app_data["vs30"].toDouble());
      num_events->setValue(app_data["numberOfEvents"].toInt());

      model_inputs_->addRow(new QLabel(tr("Moment Magnitude")), moment_mag);
      model_inputs_->addRow(
          new QLabel(tr("Closest-to-Site Rupture Distance [km]")), rupt_dist);
      model_inputs_->addRow(
          new QLabel(tr("Average shear-wave velocity for top 30 meters [m/s]")),
          vs30);
      model_inputs_->addRow(new QLabel(tr("Number of events to generate")),
                            num_events);
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

  if (model_selection_->currentText() == "Vlachos et al. (2018)") {
    app_data.insert("modelName", "vlachos2018");
    app_data.insert(
        "momentMagnitude",
        static_cast<QDoubleSpinBox*>(
            (model_inputs_->itemAt(0, QFormLayout::FieldRole))->widget())
            ->value());
    app_data.insert(
        "ruptureDist",
        static_cast<QDoubleSpinBox*>(
            (model_inputs_->itemAt(1, QFormLayout::FieldRole))->widget())
            ->value());
    app_data.insert(
        "vs30",
        static_cast<QDoubleSpinBox*>(
            (model_inputs_->itemAt(2, QFormLayout::FieldRole))->widget())
            ->value());
    app_data.insert(
        "numberOfEvents",
        static_cast<QSpinBox*>(
            (model_inputs_->itemAt(3, QFormLayout::FieldRole))->widget())
            ->value());
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
    model_description_->setText(tr(
        "This model implements the method described in Vlachos et\nal. (2018) "
        "- \"Predictive model for site specific simulation of\nground motions "
        "based on earthquake scenarions\""));

    // Delete previous entries
    while (model_inputs_->rowCount() > 0) {
      model_inputs_->removeRow(0);
    }

    QDoubleSpinBox * moment_mag = new QDoubleSpinBox();
    QDoubleSpinBox * rupt_dist = new QDoubleSpinBox();
    QDoubleSpinBox * vs30 = new QDoubleSpinBox();
    QSpinBox * num_events = new QSpinBox();
    moment_mag->setRange(0.0, std::numeric_limits<double>::infinity());
    rupt_dist->setRange(0.0, std::numeric_limits<double>::infinity());
    vs30->setRange(0.0, std::numeric_limits<double>::infinity());
    num_events->setRange(1, 900000);

    model_inputs_->addRow(new QLabel(tr("Moment Magnitude")), moment_mag);
    model_inputs_->addRow(
        new QLabel(tr("Closest-to-Site Rupture Distance [km]")), rupt_dist);
    model_inputs_->addRow(
        new QLabel(tr("Average shear-wave velocity for top 30 meters [m/s]")),
        vs30);
    model_inputs_->addRow(new QLabel(tr("Number of events to generate")), num_events);    
  } else if (model == "Testing") {
    model_description_->setText(tr("This is a test"));
    
    // Delete previous entries
    while (model_inputs_->rowCount() > 0) {
      model_inputs_->removeRow(0);
    }    
  } else {
    qDebug() << "ERROR: In StochasticMotionInput::modelSelectionChanged: "
                "Unknown selection: "
             << model << "\n";
  }
}

void StochasticMotionInput::errorMessage(QString message) {
  emit sendErrorMessage(message);
}
