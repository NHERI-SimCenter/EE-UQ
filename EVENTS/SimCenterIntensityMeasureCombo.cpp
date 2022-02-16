/* *****************************************************************************
Copyright (c) 2016-2022, The Regents of the University of California (Regents).
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

// Written by: Kuanshi Zhong, Stevan Gavrilovic, Frank McKenna


#include "SimCenterIntensityMeasureCombo.h"
#include "Utils/PythonProgressDialog.h"

#include <QStandardItem>
#include <QMetaEnum>


SimCenterIntensityMeasureCombo::SimCenterIntensityMeasureCombo(SimCenterEQ::IntensityMeasure::Type imType, QString comboName,  QWidget* parent)
    : QComboBox(parent), type(imType), name(comboName)
{
    this->setObjectName(comboName);
    this->populateComboText(imType);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
}


QString SimCenterIntensityMeasureCombo::getCurrentIMString(void)
{
    return imEnumToString(this->currentData().value<SimCenterEQ::IntensityMeasure::Type>());
}


bool SimCenterIntensityMeasureCombo::setCurrentIMString(const QString& im)
{
    SimCenterEQ::IntensityMeasure::Type imType = imStringToEnum<SimCenterEQ::IntensityMeasure::Type>(im);
    auto res = setCurrentIM(imType);
    return res;
}


bool SimCenterIntensityMeasureCombo::setCurrentIM(SimCenterEQ::IntensityMeasure::Type imType)
{
    auto index = this->findData(imType);
    this->setCurrentIndex(index);
    if(index == -1)
        return false;
    return true;
}


template<typename IMEnum> QString SimCenterIntensityMeasureCombo::imEnumToString(IMEnum enumValue)
{
    return QString(QMetaEnum::fromType<IMEnum>().valueToKey(enumValue));
}


template<typename IMEnum> IMEnum SimCenterIntensityMeasureCombo::imStringToEnum(QString imString)
{
    return (IMEnum)QMetaEnum::fromType<IMEnum>().keyToValue(imString.toStdString().c_str());
}


void SimCenterIntensityMeasureCombo::addParentItem(const QString& text)
{
    QStandardItem* item = new QStandardItem(text);
    item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
    item->setData("parent", Qt::AccessibleDescriptionRole);

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);

    QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}


void SimCenterIntensityMeasureCombo::addChildItem(const QString& text, const QVariant& data)
{
    QStandardItem* item = new QStandardItem(QString(4, QChar(' '))+text);
    item->setData(data, Qt::UserRole);
    item->setData("child", Qt::AccessibleDescriptionRole);

    QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}


void SimCenterIntensityMeasureCombo::populateComboText(const SimCenterEQ::IntensityMeasure::Type imType)
{
    switch(imType) {
    case SimCenterEQ::IntensityMeasure::Type::PEAK :

        this->addItem("Undefined", SimCenterEQ::IntensityMeasure::Type::UNDEFINED);
        this->addItem("Peak Ground Acceleration", SimCenterEQ::IntensityMeasure::Type::PGA);
        this->addItem("Peak Ground Velocity", SimCenterEQ::IntensityMeasure::Type::PGV);
        this->addItem("Peak Ground Displacement", SimCenterEQ::IntensityMeasure::Type::PGD);
        break;

    case SimCenterEQ::IntensityMeasure::Type::SPECTRUM :
        this->addItem("Undefined", SimCenterEQ::IntensityMeasure::Type::UNDEFINED);
        this->addItem("Pseudo Spectral Acceleration", SimCenterEQ::IntensityMeasure::Type::PSA);
        this->addItem("Pseudo Spectral Velocity", SimCenterEQ::IntensityMeasure::Type::PSV);
        this->addItem("Spectral Displacement", SimCenterEQ::IntensityMeasure::Type::PSD);
        this->addItem("SaRatio", SimCenterEQ::IntensityMeasure::Type::SaRatio);
        break;

    case SimCenterEQ::IntensityMeasure::Type::CUMULATIVE :
        this->addItem("Undefined", SimCenterEQ::IntensityMeasure::Type::UNDEFINED);
        this->addItem("Arias Intensity", SimCenterEQ::IntensityMeasure::Type::Ia);
        this->addItem("5-75% Signficiant Duration", SimCenterEQ::IntensityMeasure::Type::DS575);
        this->addItem("5-95% Signficiant Duration", SimCenterEQ::IntensityMeasure::Type::DS595);
        break;

    default :
        this->addItem("Undefined", SimCenterEQ::IntensityMeasure::Type::UNDEFINED);

        this->insertSeparator(this->count());
        this->addParentItem("Peak");
        this->addChildItem("Peak Ground Acceleration", SimCenterEQ::IntensityMeasure::Type::PGA);
        this->addChildItem("Peak Ground Velocity", SimCenterEQ::IntensityMeasure::Type::PGV);
        this->addChildItem("Peak Ground Displacement", SimCenterEQ::IntensityMeasure::Type::PGD);

        this->insertSeparator(this->count());
        this->addParentItem("Spectrum");
        this->addChildItem("Pseudo Spectral Acceleration", SimCenterEQ::IntensityMeasure::Type::PSA);
        this->addChildItem("Pseudo Spectral Velocity", SimCenterEQ::IntensityMeasure::Type::PSV);
        this->addChildItem("Spectral Displacement", SimCenterEQ::IntensityMeasure::Type::PSD);
        this->addChildItem("SaRatio", SimCenterEQ::IntensityMeasure::Type::SaRatio);

        this->insertSeparator(this->count());
        this->addParentItem("Cumulative");
        this->addChildItem("Arias Intensity", SimCenterEQ::IntensityMeasure::Type::Ia);
        this->addChildItem("5-75% Signficiant Duration", SimCenterEQ::IntensityMeasure::Type::DS575);
        this->addChildItem("5-95% Signficiant Duration", SimCenterEQ::IntensityMeasure::Type::DS595);

        this->setCurrentIndex(0);
    }

}


QString SimCenterIntensityMeasureCombo::getName() const
{
    return name;
}
