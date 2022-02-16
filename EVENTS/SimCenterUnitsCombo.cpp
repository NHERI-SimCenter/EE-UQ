/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
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

// Written by: Stevan Gavrilovic, Frank McKenna


#include "SimCenterUnitsCombo.h"
#include "Utils/PythonProgressDialog.h"

#include <QStandardItem>
#include <QMetaEnum>

SimCenterUnitsCombo::SimCenterUnitsCombo(SimCenter::Unit::Type unitType, QString comboName,  QWidget* parent) : QComboBox(parent), type(unitType), name(comboName)
{
    this->setObjectName(comboName);
    this->populateComboText(unitType);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
}


QString SimCenterUnitsCombo::getCurrentUnitString(void)
{
    return unitEnumToString(this->currentData().value<SimCenter::Unit::Type>());
}


bool SimCenterUnitsCombo::setCurrentUnitString(const QString& unit)
{
    SimCenter::Unit::Type unitType = unitStringToEnum<SimCenter::Unit::Type>(unit);

    auto res = setCurrentUnit(unitType);

    if(!res)
        PythonProgressDialog::getInstance()->appendErrorMessage("Could not find index of unit " + unit);

    return res;
}


bool SimCenterUnitsCombo::setCurrentUnit(SimCenter::Unit::Type unitType)
{
    auto index = this->findData(unitType);

    this->setCurrentIndex(index);

    if(index == -1)
        return false;

    return true;
}


template<typename UnitEnum> QString SimCenterUnitsCombo::unitEnumToString(UnitEnum enumValue)
{
    return QString(QMetaEnum::fromType<UnitEnum>().valueToKey(enumValue));
}


template<typename UnitEnum> UnitEnum SimCenterUnitsCombo::unitStringToEnum(QString unitString)
{
    return (UnitEnum)QMetaEnum::fromType<UnitEnum>().keyToValue(unitString.toStdString().c_str());
}


void SimCenterUnitsCombo::addParentItem(const QString& text)
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


void SimCenterUnitsCombo::addChildItem( const QString& text, const QVariant& data )
{
    QStandardItem* item = new QStandardItem(QString(4, QChar( ' ' ))+text);
    item->setData(data, Qt::UserRole);
    item->setData("child", Qt::AccessibleDescriptionRole);

    QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}


void SimCenterUnitsCombo::populateComboText(const SimCenter::Unit::Type unitType)
{
    switch(unitType) {
    case SimCenter::Unit::Type::LENGTH :

        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->addItem("Millimeters", SimCenter::Unit::Type::mm);
        this->addItem("Centimeters", SimCenter::Unit::Type::cm);
        this->addItem("Meters", SimCenter::Unit::Type::m);
        this->addItem("Kilometers", SimCenter::Unit::Type::km);
        this->addItem("Inches", SimCenter::Unit::Type::inch);
        this->addItem("Feet", SimCenter::Unit::Type::ft);
        this->addItem("Miles", SimCenter::Unit::Type::mi);

        break;
    case SimCenter::Unit::Type::FORCE :
        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->addItem("Newtons", SimCenter::Unit::Type::N);
        this->addItem("Kilonewtons", SimCenter::Unit::Type::kN);
        this->addItem("Pounds", SimCenter::Unit::Type::lb);
        this->addItem("Kips", SimCenter::Unit::Type::kips);

        break;
    case SimCenter::Unit::Type::PRESSURE :
        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->addItem("Pascal", SimCenter::Unit::Type::Pa);
        this->addItem("Megapascal", SimCenter::Unit::Type::MPa);
        this->addItem("Gigapascal", SimCenter::Unit::Type::GPa);
        this->addItem("bar", SimCenter::Unit::Type::bar);
        this->addItem("Atmosphere", SimCenter::Unit::Type::atm);

        break;
    case SimCenter::Unit::Type::TIME :
        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->addItem("Seconds", SimCenter::Unit::Type::sec);
        this->addItem("Minutes", SimCenter::Unit::Type::min);
        this->addItem("Hours", SimCenter::Unit::Type::hr);
        this->addItem("Days", SimCenter::Unit::Type::hr);

        break;
    case SimCenter::Unit::Type::VELOCITY :
        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->addItem("Centimeters per second", SimCenter::Unit::Type::cmps);
        this->addItem("Meters per second", SimCenter::Unit::Type::mps);
        this->addItem("Kilometers per hour", SimCenter::Unit::Type::kph);
        this->addItem("Feet per second", SimCenter::Unit::Type::fps);
        this->addItem("Miles per hour", SimCenter::Unit::Type::mph);
        this->addItem("Knots", SimCenter::Unit::Type::kts);

        break;
    case SimCenter::Unit::Type::ACCEL :
        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->addItem("Centimeters per second squared", SimCenter::Unit::Type::cmps2);
        this->addItem("Meters per second squared", SimCenter::Unit::Type::mps2);
        this->addItem("Inches per second squared", SimCenter::Unit::Type::inchps2);
        this->addItem("Feet per second squared", SimCenter::Unit::Type::ftps2);
        this->addItem("Gravitational constant (g)", SimCenter::Unit::Type::g);
        this->addItem("Natural logarithm of gravitational constant ln(g)", SimCenter::Unit::Type::lng);
        this->addItem("Percent of gravitational constant (%g)", SimCenter::Unit::Type::pctg);

        break;
    default :

        this->addItem("Undefined", SimCenter::Unit::Type::UNDEFINED);
        this->insertSeparator(this->count());

        this->addParentItem("Length");

        this->addChildItem("Millimeters", SimCenter::Unit::Type::mm);
        this->addChildItem("Centimeters", SimCenter::Unit::Type::cm);
        this->addChildItem("Meters", SimCenter::Unit::Type::m);
        this->addChildItem("Kilometers", SimCenter::Unit::Type::km);
        this->addChildItem("Inches", SimCenter::Unit::Type::inch);
        this->addChildItem("Feet", SimCenter::Unit::Type::ft);
        this->addChildItem("Miles", SimCenter::Unit::Type::mi);

        this->insertSeparator(this->count());
        this->addParentItem("Force");

        this->addChildItem("Newtons", SimCenter::Unit::Type::N);
        this->addChildItem("Kilonewtons", SimCenter::Unit::Type::kN);
        this->addChildItem("Pounds", SimCenter::Unit::Type::lb);
        this->addChildItem("Kips", SimCenter::Unit::Type::kips);

        this->insertSeparator(this->count());
        this->addParentItem("Pressure");

        this->addChildItem("Pascal", SimCenter::Unit::Type::Pa);
        this->addChildItem("Megapascal", SimCenter::Unit::Type::MPa);
        this->addChildItem("Gigapascal", SimCenter::Unit::Type::GPa);
        this->addChildItem("bar", SimCenter::Unit::Type::bar);
        this->addChildItem("Atmosphere", SimCenter::Unit::Type::atm);

        this->insertSeparator(this->count());
        this->addParentItem("Time");

        this->addChildItem("Seconds", SimCenter::Unit::Type::sec);
        this->addChildItem("Minutes", SimCenter::Unit::Type::min);
        this->addChildItem("Hours", SimCenter::Unit::Type::hr);
        this->addChildItem("Days", SimCenter::Unit::Type::hr);

        this->insertSeparator(this->count());
        this->addParentItem("Velocity");

        this->addChildItem("Centimeters per second", SimCenter::Unit::Type::cmps);
        this->addChildItem("Meters per second", SimCenter::Unit::Type::mps);
        this->addChildItem("Kilometers per hour", SimCenter::Unit::Type::kph);
        this->addChildItem("Feet per second", SimCenter::Unit::Type::fps);
        this->addChildItem("Miles per hour", SimCenter::Unit::Type::mph);
        this->addChildItem("Knots", SimCenter::Unit::Type::kts);

        this->insertSeparator(this->count());
        this->addParentItem("Acceleration");

        this->addChildItem("Centimeters per second squared", SimCenter::Unit::Type::cmps2);
        this->addChildItem("Meters per second squared", SimCenter::Unit::Type::mps2);
        this->addChildItem("Inches per second squared", SimCenter::Unit::Type::inchps2);
        this->addChildItem("Feet per second squared", SimCenter::Unit::Type::ftps2);
        this->addChildItem("Gravitational constant (g)", SimCenter::Unit::Type::g);
        this->addChildItem("Natural logarithm of gravitational constant ln(g)", SimCenter::Unit::Type::lng);
        this->addChildItem("Percent of gravitational constant (%g)", SimCenter::Unit::Type::pctg);

        this->insertSeparator(this->count());
        this->addParentItem("Dimensionless");
        this->addChildItem("Unitless (scalar)", SimCenter::Unit::Type::scalar);

        this->setCurrentIndex(0);
    }

}


QString SimCenterUnitsCombo::getName() const
{
    return name;
}
