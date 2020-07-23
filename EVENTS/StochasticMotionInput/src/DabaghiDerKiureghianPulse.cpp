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
#include <SimCenterWidget.h>

#include "DabaghiDerKiureghianPulse.h"

DabaghiDerKiureghianPulse::DabaghiDerKiureghianPulse(
    RandomVariablesContainer* random_variables, QWidget* parent)
    : StochasticModelWidget(random_variables, parent) {
  // Initialize member variables
  faulting_ = new QComboBox();
  faulting_->setObjectName("Type of faulting");
  faulting_->addItem(tr("Strike-Slip"));
  faulting_->addItem(tr("Reverse and Reverse-Oblique"));

  sim_type_ = new QComboBox();
  sim_type_->setObjectName("Pulse-like proportion of motions");
  sim_type_->addItem(tr("Pulse-like and no pulse-like"));  
  sim_type_->addItem(tr("Only pulse-like"));
  sim_type_->addItem(tr("No pulse-like"));
  
  moment_magnitude_ = new LineEditRV(random_variables);
  depth_to_rupt_ = new LineEditRV(random_variables);  
  rupture_dist_ = new LineEditRV(random_variables);
  vs30_ = new LineEditRV(random_variables);
  s_or_d_ = new LineEditRV(random_variables);
  theta_or_phi_ = new LineEditRV(random_variables);  
  truncate_ = new QRadioButton("Truncate and baseline correct synthetic motion");
  truncate_->setAutoExclusive(false);
  truncate_->setChecked(true);  
  seed_ = new QSpinBox();
  seed_->setMinimum(1);
  seed_->setMaximum(2147483647);
  seed_->setValue(500);  
  seed_->setEnabled(false);
  use_seed_ = new QRadioButton("Provide seed value");
  use_seed_->setAutoExclusive(false);
  use_seed_->setChecked(false);
  
  parameters_ = new QFormLayout();
  parameters_->addRow(new QLabel(tr("Moment Magnitude")), moment_magnitude_);
  parameters_->addRow(new QLabel(tr("Depth to Top of Rupture Plane [km]")), depth_to_rupt_);
  parameters_->addRow(new QLabel(tr("Closest-to-Site Rupture Distance [km]")),
                      rupture_dist_);
  parameters_->addRow(
      new QLabel(tr("Average shear-wave velocity for top 30 meters [m/s]")),
      vs30_);
  parameters_->addRow(new QLabel(tr("s or d [km] (Input larger of the two)")),
                      s_or_d_);
  parameters_->addRow(
      new QLabel(
          tr("Theta or Phi [degrees] (Input value corresponding to s or d)")),
      theta_or_phi_);

  // Add description label
  model_description_ = new QLabel(
      tr("This model implements the method described in Dabaghi & Der "
         "Kiureghian (2018) \n\"Simulation of orthogonal horizontal "
         "components of near-fault "
         "motion for specified\nearthquake source and site characteristics\""));
  //model_description_->setStyleSheet("QLabel { color : gray; }");

  // Add image of directivity params
  directivity_params_ = new QLabel(
      "Geometric directivity parameters (Dabaghi & Der Kiureghian, 2018)");
  QPixmap directivity_image(":resources/directivityParameters.png");
  //  QSize sizePixmap = directivity_image.size();
  //  QPixmap scaledPixmap = directivity_image.scaled(QSize(sizePixmap.height()*.75, sizePixmap.width()*.75));
  directivity_params_->setPixmap(directivity_image);
  directivity_params_->show();

  // Construct required layouts
  QHBoxLayout* layout = new QHBoxLayout();
  QVBoxLayout* inputs_layout = new QVBoxLayout;
  QHBoxLayout* seed_layout = new QHBoxLayout();
  QHBoxLayout* truncate_layout = new QHBoxLayout();  
  QHBoxLayout* parameters_layout = new QHBoxLayout();
  QHBoxLayout* faulting_layout = new QHBoxLayout();
  QHBoxLayout* sim_type_layout = new QHBoxLayout();
  QHBoxLayout* image_layout = new QHBoxLayout();    
  
  // Add widgets to layouts and layouts to this
  seed_layout->addWidget(use_seed_);
  seed_layout->addWidget(seed_);
  seed_layout->addStretch();
  truncate_layout->addWidget(truncate_);
  truncate_layout->addStretch();  
  parameters_layout->addLayout(parameters_);
  parameters_layout->addStretch();
  faulting_layout->addWidget(faulting_);
  faulting_layout->addStretch();
  sim_type_layout->addWidget(sim_type_);
  sim_type_layout->addStretch();
  image_layout->addWidget(directivity_params_);
  image_layout->addStretch();  
  inputs_layout->addWidget(model_description_);
  inputs_layout->addLayout(faulting_layout);
  inputs_layout->addLayout(sim_type_layout);
  inputs_layout->addLayout(parameters_layout);
  inputs_layout->addLayout(truncate_layout);
  inputs_layout->addLayout(seed_layout);
  inputs_layout->addStretch();
  
  layout->addLayout(inputs_layout);
  layout->addLayout(image_layout);
  layout->setStretch(0, 2);
  layout->setStretch(1, 3);
  layout->setSpacing(10);
  
  this->setLayout(layout);

  // Connect slots
  connect(use_seed_, &QRadioButton::toggled, this,
          &DabaghiDerKiureghianPulse::provideSeed);
}

bool DabaghiDerKiureghianPulse::outputToJSON(QJsonObject& jsonObject) {
  bool result = true;

  auto fault_type = faulting_->currentText();
  if (fault_type == "Strike-Slip") {
    jsonObject.insert("faultType", "StrikeSlip");
  } else if (fault_type == "Reverse and Reverse-Oblique") {
    jsonObject.insert("faultType", "ReverseAndRevObliq");
  }

  auto simulation_type = sim_type_->currentText();
  if (simulation_type == "Pulse-like and no pulse-like") {
    jsonObject.insert("simulationType", "PulseAndNoPulse");
  } else if (simulation_type == "Only pulse-like") {
    jsonObject.insert("simulationType", "Pulse");
  } else if (simulation_type == "No pulse-like") {
    jsonObject.insert("simulationType", "NoPulse");
  }  
  
  moment_magnitude_->outputToJSON(jsonObject, QString("momentMagnitude"));
  depth_to_rupt_->outputToJSON(jsonObject, QString("depthToRupt"));
  rupture_dist_->outputToJSON(jsonObject, QString("ruptureDist"));
  vs30_->outputToJSON(jsonObject, QString("vs30"));
  s_or_d_->outputToJSON(jsonObject, QString("sOrD"));
  theta_or_phi_->outputToJSON(jsonObject, QString("thetaOrPhi"));

  jsonObject.insert("truncate", truncate_->isChecked());

  if (use_seed_->isChecked()) {
    jsonObject.insert("seed", seed_->value());
  } else {
    jsonObject.insert("seed", "None");
  }
  
  return result;
}

bool DabaghiDerKiureghianPulse::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;

  QString fault_type = jsonObject["faultType"].toString();
  QString sim_type = jsonObject["simulationType"].toString();

  if (fault_type == "StrikeSlip") {
    faulting_->setCurrentIndex(faulting_->findText("Strike-Slip"));
  } else if (fault_type == "ReverseAndRevObliq") {
    faulting_->setCurrentIndex(
        faulting_->findText("Reverse and Reverse-Oblique"));
  }

  auto simulation_type = sim_type_->currentText();
  if (simulation_type == "PulseAndNoPulse") {
    sim_type_->setCurrentIndex(
        sim_type_->findText("Pulse-like proportion of motions"));
  } else if (simulation_type == "Pulse") {
    sim_type_->setCurrentIndex(sim_type_->findText("Only pulse-like"));
  } else if (simulation_type == "NoPulse") {
    sim_type_->setCurrentIndex(sim_type_->findText("No pulse-like"));
  }

  moment_magnitude_->inputFromJSON(jsonObject, QString("momentMagnitude"));
  depth_to_rupt_->inputFromJSON(jsonObject, QString("depthToRupt"));
  rupture_dist_->inputFromJSON(jsonObject, QString("ruptureDist"));
  vs30_->inputFromJSON(jsonObject, QString("vs30"));
  s_or_d_->inputFromJSON(jsonObject, QString("sOrD"));
  theta_or_phi_->inputFromJSON(jsonObject, QString("thetaOrPhi"));

  if (jsonObject["truncate"].toBool()) {
    truncate_->setChecked(true);
  } else {
    truncate_->setChecked(false);    
  }

  if (jsonObject.value("seed").isString()) {
    use_seed_->setChecked(false);    
  } else {
    use_seed_->setChecked(true);
    seed_->setValue(jsonObject.value("seed").toInt());    
  }

  return result;
}

void DabaghiDerKiureghianPulse::provideSeed(const bool& checked) {
  if (checked) {
    seed_->setEnabled(true);
  } else {
    seed_->setEnabled(false);
    seed_->setValue(500);
  }
}
