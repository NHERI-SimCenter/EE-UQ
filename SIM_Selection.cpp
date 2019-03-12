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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Written: fmckenna

#include "SIM_Selection.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>
#include <InputWidgetSheetSIM.h>

#include <SimCenterWidget.h>
#include <InputWidgetSheetSIM.h>
#include <OpenSeesBuildingModel.h>
#include <MDOF_BuildingModel.h>


SIM_Selection::SIM_Selection(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), bimInput(0), theRandomVariablesContainer(theRandomVariableIW)
{
    layout = new QVBoxLayout();

    QVBoxLayout *name= new QVBoxLayout;

    // text and add button at top
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *textBIM=new SectionTitle();
    textBIM->setText(tr("Building Model Input"));
    textBIM->setMinimumWidth(250);
    QSpacerItem *spacer = new QSpacerItem(50,10);

    bimSelection = new QComboBox();
    bimSelection->setMaximumWidth(200);
    bimSelection->setMinimumWidth(200);

    titleLayout->addWidget(textBIM);
    titleLayout->addItem(spacer);
    titleLayout->addWidget(bimSelection);
    titleLayout->addStretch();
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    name->addLayout(titleLayout);

    name->setSpacing(10);
    name->setMargin(0);

    //    bimSelection->addItem(tr("Spreadsheet"));  
    bimSelection->addItem(tr("MDOF"));
    bimSelection->addItem(tr("OpenSees"));

    connect(bimSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(bimSelectionChanged(QString)));

    layout->addLayout(name);
    layout->addStretch();

    this->setLayout(layout);

    // set Samlping as the default
    //    this->bimSelectionChanged(tr("Spreadsheet"));
    this->bimSelectionChanged(tr("MDOF"));
    layout->setMargin(0);
}

SIM_Selection::~SIM_Selection()
{

}


void SIM_Selection::clear(void)
{

}




bool
SIM_Selection::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    //
    // create a json object, fill it and then add this to the object with uqMethod key
    //

    //QJsonObject uq;
    if (bimInput != 0) {
        result = bimInput->outputToJSON(jsonObject);
    }
    //uq["uqType"] = bimSelection->currentText();

    //jsonObject["uqMethod"]=uq;

    return result;
}


bool
SIM_Selection::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;
    this->clear();

    //
    // get type, determine index & invoke setCurrentIndex on combobox
    //

    QString type;
    if (jsonObject.contains("type")) {
        QJsonValue theName = jsonObject["type"];
        type = theName.toString();
    } else
        return false;

    int index = 0;
    if (type == QString("SimCenterSIM")) {
       index = 2;
    } else if (type == QString("OpenSeesInput")) {
       index = 1;
    } else if (type == QString("MDOF_BuildingModel")) {
       index = 0;
    } else {
        return false;
    }

    bimSelection->setCurrentIndex(index);

    // if worked, just invoke method on new type

    if (bimInput != 0) {
        result = bimInput->inputFromJSON(jsonObject);
    }

    return result;
}


bool
SIM_Selection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    bool result = false;

    if (bimInput != 0) {
        result = bimInput->outputAppDataToJSON(jsonObject);
    }

    return result;
}


bool
SIM_Selection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    if (bimInput != 0) {
        result = bimInput->inputAppDataFromJSON(jsonObject);
    }

    return result;
}


bool
SIM_Selection::copyFiles(QString &destDir) {

    if (bimInput != 0) {
        return  bimInput->copyFiles(destDir);
    }

    return false;
}

void SIM_Selection::bimSelectionChanged(const QString &arg1)
{
    selectionChangeOK = true;

    SimCenterWidget *oldMethod = bimInput;

    if (bimInput != 0)
        layout->removeWidget(bimInput);

    //
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == QString("Spreadsheet") || arg1 == QString("SimCenterSIM")) {
        delete bimInput;
        bimInput = new InputWidgetSheetSIM(theRandomVariablesContainer);

    } else if (arg1 == QString("OpenSees") || (arg1 == QString("OpenSeesInput"))) {
        delete bimInput;
        bimInput = new OpenSeesBuildingModel(theRandomVariablesContainer);
    } else if (arg1 == QString("MDOF") || (arg1 == QString("MDOF_BuildingModel"))) {
        delete bimInput;
        bimInput = new MDOF_BuildingModel(theRandomVariablesContainer);
    } else {
        selectionChangeOK = false;
        emit sendErrorMessage("ERROR: BIM Input - no valid Method provided .. keeping old");
    }

    if (bimInput != 0) {
        this->bimWidgetChanged();
        layout->insertWidget(1, bimInput,1);
        connect(bimInput,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    }

    return;
}


void
SIM_Selection::errorMessage(QString message){
  emit sendErrorMessage(message);
}


