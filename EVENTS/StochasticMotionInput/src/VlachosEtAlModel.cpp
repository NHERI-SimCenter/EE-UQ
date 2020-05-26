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

#include "VlachosEtAlModel.h"

VlachosEtAlModel::VlachosEtAlModel(RandomVariablesContainer* random_variables,
                                   QWidget* parent)
    : StochasticModelWidget(random_variables, parent)
{
  // Initialize member variables
  moment_magnitude_ = new LineEditRV(random_variables); moment_magnitude_->setText("7.0");
  rupture_dist_ = new LineEditRV(random_variables); rupture_dist_->setText("40.0");
  vs30_ = new LineEditRV(random_variables); vs30_->setText("500.0");
  seed_ = new QSpinBox();
  seed_->setMinimum(1);
  seed_->setMaximum(2147483647);
  seed_->setValue(500);  
  seed_->setEnabled(false);
  use_seed_ = new QRadioButton("Provide seed value");
  use_seed_->setChecked(true);
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
  //model_description_->setStyleSheet("QLabel { color : gray; }");

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
  connect(use_seed_, &QRadioButton::toggled, this,
          &VlachosEtAlModel::provideSeed);
}

bool VlachosEtAlModel::outputToJSON(QJsonObject& jsonObject) {
  bool result = true;

  moment_magnitude_->outputToJSON(jsonObject, QString("momentMagnitude"));
  rupture_dist_->outputToJSON(jsonObject, QString("ruptureDist"));
  vs30_->outputToJSON(jsonObject, QString("vs30"));
  
  if (use_seed_->isChecked()) {
    jsonObject.insert("seed", seed_->value());
  } else {
    jsonObject.insert("seed", "None");
  }
  
  return result;
}

bool VlachosEtAlModel::inputFromJSON(QJsonObject& jsonObject) {
  bool result = true;

  moment_magnitude_->inputFromJSON(jsonObject, QString("momentMagnitude"));
  rupture_dist_->inputFromJSON(jsonObject, QString("ruptureDist"));
  vs30_->inputFromJSON(jsonObject, QString("vs30"));

  if (jsonObject.value("seed").isString()) {
    use_seed_->setChecked(false);    
  } else {
    use_seed_->setChecked(true);
    seed_->setValue(jsonObject.value("seed").toInt());    
  }

  return result;
}

void VlachosEtAlModel::provideSeed(const bool& checked) {
  if (checked) {
    seed_->setEnabled(true);
  } else {
    seed_->setEnabled(false);
    seed_->setValue(500);
  }
}
