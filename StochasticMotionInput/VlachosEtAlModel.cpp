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
#include <SimCenterWidget.h>

#include "VlachosEtAlModel.h"

VlachosEtAlModel::VlachosEtAlModel(RandomVariableInputWidget* random_variables,
                                   QWidget* parent)
    : StochasticModelWidget(random_variables, parent)
{
  // Initialize member variables
  moment_magnitude_ = new QLineEdit();
  rupture_dist_ = new QLineEdit();
  vs30_ = new QLineEdit();
  seed_ = new QSpinBox();
  seed_->setEnabled(false);
  use_seed_ = new QRadioButton("Provide seed value");
  use_seed_->setEnabled(false);
  parameters_ = new QFormLayout();
  parameters_->addRow(new QLabel(tr("Moment Magnitude")), moment_magnitude_);
  parameters_->addRow(new QLabel(tr("Closest-to-Site Rupture Distance [km]")),
                      rupture_dist_);
  parameters_->addRow(
      new QLabel(tr("Average shear-wave velocity for top 30 meters [m/s]")),
      vs30_);

  // Add description label
  model_description_ = new QLabel(
      tr("This model implements the method described in Vlachos et\nal. (2018) "
         "- \"Predictive model for site specific simulation of\nground motions "
         "based on earthquake scenarions\""));
  model_description_->setStyleSheet("QLabel { color : gray; }");    

  // Construct required layouts
  QVBoxLayout* layout = new QVBoxLayout();
  QHBoxLayout* seed_layout = new QHBoxLayout();
  QHBoxLayout* parameters_layout = new QHBoxLayout();

  // Add widgets to layouts and layouts to this
  seed_layout->addWidget(use_seed_);
  seed_layout->addWidget(seed_);
  seed_layout->addStretch();
  parameters_layout->addLayout(parameters_);
  parameters_layout->addStretch();
  layout->addWidget(model_description_);
  layout->addLayout(parameters_layout);
  layout->addLayout(seed_layout);
  layout->addStretch();
  this->setLayout(layout);

  // Connect slots
  connect(moment_magnitude_, &QLineEdit::editingFinished, this,
          &VlachosEtAlModel::updateMoment);
  connect(rupture_dist_, &QLineEdit::editingFinished, this,
          &VlachosEtAlModel::updateRuptDist);
  connect(vs30_, &QLineEdit::editingFinished, this,
          &VlachosEtAlModel::updateVs30);
  connect(use_seed_, &QRadioButton::toggled, this,
          &VlachosEtAlModel::provideSeed);
  connect(seed_, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &VlachosEtAlModel::updateSeed);
}

bool VlachosEtAlModel::outputToJSON(QJsonObject& jsonObject) {
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

bool VlachosEtAlModel::inputFromJSON(QJsonObject& jsonObject) {
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

void VlachosEtAlModel::updateMoment(const QString& moment) {
  bool conversion_check;
  double moment_mag = moment.toDouble(&conversion_check);
  moment_magnitude_->setText(moment);

  // Check if moment magnitude is a random variable
  if (!conversion_check) {
    QStringList random_var{moment, "7.0"};
    rv_input_widget_->addConstantRVs(random_var);    
  }
}

void VlachosEtAlModel::updateRuptDist(const QString& rupt_dist) {
  bool conversion_check;
  double rupture_dist = rupt_dist.toDouble(&conversion_check);
  rupture_dist_->setText(rupt_dist);

  // Check if moment magnitude is a random variable
  if (!conversion_check) {
    QStringList random_var{rupt_dist, "200.0"};
    rv_input_widget_->addConstantRVs(random_var);    
  }  
}

void VlachosEtAlModel::updateVs30(const QString& vs30) {
  bool conversion_check;
  double vs30_value = vs30.toDouble(&conversion_check);
  vs30_->setText(vs30);

  // Check if moment magnitude is a random variable
  if (!conversion_check) {
    QStringList random_var{vs30, "500.0"};
    rv_input_widget_->addConstantRVs(random_var);    
  }
}

void VlachosEtAlModel::provideSeed(const bool& checked) {
  if (checked) {
    seed_->setEnabled(true);
  } else {
    seed_->setEnabled(false);
    seed_->setValue(500);
  }
}

void VlachosEtAlModel::updateSeed(int seed_val) {
  seed_->setValue(seed_val);
}
