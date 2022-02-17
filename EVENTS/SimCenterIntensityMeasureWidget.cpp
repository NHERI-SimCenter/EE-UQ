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


#include "SimCenterIntensityMeasureWidget.h"
#include "SimCenterIntensityMeasureCombo.h"
#include "SimCenterUnitsCombo.h"
#include "Utils/PythonProgressDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <sectiontitle.h>
#include <QLineEdit>

SimCenterIntensityMeasureWidget::SimCenterIntensityMeasureWidget(QWidget* parent)
    : SimCenterWidget(parent)
{
    vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Intensity Measure Calculation"));
    title->setMinimumWidth(250);

    QPushButton *addButton = new QPushButton();
    addButton->setMinimumWidth(60);
    addButton->setMaximumWidth(60);
    addButton->setText(tr("Add"));

    QPushButton *removeButton = new QPushButton();
    removeButton->setMinimumWidth(60);
    removeButton->setMaximumWidth(60);
    removeButton->setText(tr("Remove"));

    hLayout->addWidget(title);
    hLayout->addWidget(addButton);
    hLayout->addSpacing(10);
    hLayout->addWidget(removeButton);
    hLayout->addStretch();

    QGroupBox *imBox = new QGroupBox;
    imLayout = new QVBoxLayout(this);
    imBox->setLayout(imLayout);
    imLayout->addStretch();
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setWidget(imBox);
    sa->setMinimumHeight(200);
    sa->setMaximumHeight(200);

    vLayout->addLayout(hLayout);
    vLayout->addWidget(sa);

    this->setLayout(vLayout);

    connect(addButton, SIGNAL(pressed()), this, SLOT(addIMItem()));
    connect(removeButton, SIGNAL(pressed()), this, SLOT(removeIMItem()));
}


SimCenterIM::SimCenterIM(SimCenterIntensityMeasureCombo *theIM, SimCenterUnitsCombo *theUnit, QWidget *parent)
    : SimCenterWidget(parent), myIM(theIM), myUnit(theUnit)
{
    button = new QRadioButton();
    imUnitLayout = new QHBoxLayout();
    imUnitLayout->addWidget(button);
    imUnitLayout->addWidget(myIM);
    imUnitLayout->addWidget(myUnit);
    QHBoxLayout *periodLayout = new QHBoxLayout;
    QLabel *periodLabel = new QLabel(tr("Periods:"));
    periodLine = new QLineEdit("0.5,1.0,2.0");
    QRegExp regExpAllow("^([1-9][0-9]*|[1-9]*\\.[0-9]*|0\\.[0-9]*)*(([ ]*,[ ]*){0,1}([[1-9]*\\.[0-9]*|[1-9][0-9]*|0\\.[0-9]*))*");
    LEValidator = new QRegExpValidator(regExpAllow,this);
    periodLayout->addWidget(periodLabel);
    periodLayout->addWidget(periodLine);
    periodLayout->setMargin(0);
    myPeriods = new QGroupBox();
    myPeriods->setLayout(periodLayout);
    myPeriods->setVisible(false);
    imUnitLayout->addWidget(myPeriods);
    imUnitLayout->addStretch();
    this->setLayout(imUnitLayout);

    connect(myIM, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(handleIMChanged(const QString&)));
}


SimCenterIM::~SimCenterIM()
{

}


void SimCenterIM::handleIMChanged(const QString& newIM)
{
    if ((newIM.contains("Spectral")) || (newIM.contains("SaRatio")))
        myPeriods->setVisible(true);
    else
        myPeriods->setVisible(false);
}


QString SimCenterIM::checkPeriodsValid(const QString& input) const
{

    QString validInput = input;
    if(validInput.isEmpty())
        return validInput;
    int pos = 0;
    if(LEValidator->validate(const_cast<QString&>(input), pos) != 1)
    {
        validInput = QStringRef(&input, 0, pos-1).toString();
        qDebug()<<"pos"<<pos<<" : "<<validInput;
        periodLine->setText(validInput);
    }
    return validInput;
}

void SimCenterIntensityMeasureWidget::clear(void)
{
    QLayoutItem *child;
    while ((child = imLayout->takeAt(0)) != nullptr)
    {
        auto widget = child->widget();
        if (widget)
            widget->setParent(nullptr);
        delete child;
    }
}


bool SimCenterIntensityMeasureWidget::outputToJSON(QJsonObject &jsonObject)
{
    auto numItems = this->getNumberOfIM();
    if(numItems<1)
        return true;
    QJsonObject imObj;
    for(int i = 0; i<numItems; ++i)
    {
        QJsonObject curObj;
        // IM
        QLayoutItem *child = imLayout->itemAt(i);
        auto curIMUnit = dynamic_cast<SimCenterIM*>(child->widget());
        auto widget = dynamic_cast<SimCenterIM*>(child->widget())->myIM;
        QString im;
        if (widget)
        {
            auto name = widget->getName();
            if(name.isEmpty())
                return false;
            im = widget->getCurrentIMString();
            // Return false if unit undefined
            if(im.compare("UNDEFINED") == 0)
            {
                PythonProgressDialog::getInstance()->appendErrorMessage("Warning IM undefined! Please set IM");
                return false;
            }
        }
        // unit
        auto unit_widget = dynamic_cast<SimCenterIM*>(child->widget())->myUnit;
        if (unit_widget)
        {
            auto name = unit_widget->getName();
            if(name.isEmpty())
                return false;
            auto unit = unit_widget->getCurrentUnitString();
            // Return false if unit undefined
            if(unit.compare("UNDEFINED") == 0)
            {
                PythonProgressDialog::getInstance()->appendErrorMessage("Warning unit undefined! Please set unit");
                return false;
            }
            curObj["Unit"] = unit;
        }
        // period if
        if ((im.startsWith("Pseudo")) || (im.compare("SaRatio")==0))
        {
            auto periodsString = curIMUnit->periodLine->text();
            if (periodsString.isEmpty())
            {
                PythonProgressDialog::getInstance()->appendErrorMessage("Error periods not defined for "+im);
                return false;
            }
            auto parsedPeriods = curIMUnit->checkPeriodsValid(periodsString);
            parsedPeriods.remove(" ");
            QJsonArray periodArray;
            auto periodList = parsedPeriods.split(",");
            if ((im.compare("SaRatio")==0) && (periodList.size() != 3))
            {
                PythonProgressDialog::getInstance()->appendErrorMessage("Error three periods for SaRatio Ta, T1, and Tb");
                return false;
            }
            for (int i=0; i<periodList.size(); i++)
                periodArray.append(periodList.at(i).toDouble());
            curObj["Periods"] = periodArray;
        }
        jsonObject.insert(im, curObj);
    }
    return true;
}


bool SimCenterIntensityMeasureWidget::inputFromJSON(QJsonObject &jsonObject)
{
    auto imObj = jsonObject.value("IntensityMeasure").toObject();
    if(imObj.isEmpty())
        return false;
    int i = 1;
    foreach(const QString& key, imObj.keys())
    {
        auto im = key;
        QString imName = "IM"+QString::number(i);
        auto imCombo = this->imFindChild(imName);
        if(imCombo == nullptr)
            return false;
        else
        {
            auto res = imCombo->setCurrentIMString(im);
            if(!res)
                return false;
        }

        auto unit = imObj.value(key).toString();
        QString unitName = "Unit"+QString::number(i);
        auto unitCombo = this->unitFindChild(key);
        if(unitCombo == nullptr)
            return false;
        else
        {
            auto res = unitCombo->setCurrentUnitString(unit);
            if(!res)
                return false;
        }
        i = i + 1;
    }
    return true;
}


void SimCenterIntensityMeasureWidget::reset(void)
{

}


void SimCenterIntensityMeasureWidget::addIMItem()
{
    auto i = this->getNumberOfIM();
    QString imName = "IM"+QString::number(i+1);
    QString unitName = "Unit"+QString::number(i+1);
    SimCenterIntensityMeasureCombo *imCombo = new SimCenterIntensityMeasureCombo(SimCenterEQ::IntensityMeasure::ALL,imName);
    SimCenterUnitsCombo *unitCombo = new SimCenterUnitsCombo(SimCenter::Unit::ALL,unitName);
    SimCenterIM *imUnitCombo = new SimCenterIM(imCombo, unitCombo);
    imLayout->insertWidget(i, imUnitCombo);
}


void SimCenterIntensityMeasureWidget::removeIMItem()
{
    auto numIM = this->getNumberOfIM();
    for (int i = numIM-1; i >= 0; i--) {
        QLayoutItem *curItem = imLayout->itemAt(i);
        auto curWidget = dynamic_cast<SimCenterIM*>(curItem->widget());
        if (curWidget->button->isChecked()) {
            imLayout->removeWidget(curWidget);
            curWidget->setParent(0);
            delete curWidget;
        }
    }
}


SimCenterIntensityMeasureCombo* SimCenterIntensityMeasureWidget::imFindChild(const QString& name)
{
    auto numItems = this->getNumberOfIM();
    for(int i = 0; i<numItems; ++i)
    {
        QLayoutItem *child = imLayout->itemAt(i);
        auto widget = dynamic_cast<SimCenterIntensityMeasureCombo*>(child->widget());
        if (widget)
        {
            auto widgetName = widget->getName();
            if(widgetName.compare(name) == 0)
                return widget;
        }
    }
    return nullptr;
}


SimCenterUnitsCombo* SimCenterIntensityMeasureWidget::unitFindChild(const QString& name)
{
    auto numItems = this->getNumberOfIM();
    for(int i = 0; i<numItems; ++i)
    {
        QLayoutItem *child = imLayout->itemAt(i);
        auto widget = dynamic_cast<SimCenterUnitsCombo*>(child->widget());
        if (widget)
        {
            auto widgetName = widget->getName();
            if(widgetName.compare(name) == 0)
                return widget;
        }
    }
    return nullptr;
}


int SimCenterIntensityMeasureWidget::getNumberOfIM(void)
{
    return imLayout->count()-1;
}


int SimCenterIntensityMeasureWidget::setIM(const QString& parameterName, const QString& im)
{
    auto widget = this->imFindChild(parameterName);
    if(widget)
    {
        auto res = widget->setCurrentIMString(im);
        if(!res)
            return -1;
    }
    else
        return -1;
    return 0;
}


int SimCenterIntensityMeasureWidget::setUnit(const QString& parameterName, const QString& unit)
{
    auto widget = this->unitFindChild(parameterName);
    if(widget)
    {
        auto res = widget->setCurrentUnitString(unit);
        if(!res)
            return -1;
    }
    else
        return -1;
    return 0;
}


QList<QString> SimCenterIntensityMeasureWidget::getParameterNames()
{
    QList<QString> paramList;
    auto numItems = this->getNumberOfIM();
    for(int i = 0; i<numItems; ++i)
    {
        QLayoutItem *child = imLayout->itemAt(i);
        auto widget = dynamic_cast<SimCenterIntensityMeasureCombo*>(child->widget());
        if (widget)
        {
            auto widgetName = widget->getName();
            paramList.append(widgetName);
        }
    }
    return paramList;
}
