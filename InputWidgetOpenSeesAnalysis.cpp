// Written: fmckenna

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

#include "InputWidgetOpenSeesAnalysis.h"
#include <RandomVariableInputWidget.h>


#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QGridLayout>


InputWidgetOpenSeesAnalysis::InputWidgetOpenSeesAnalysis(RandomVariableInputWidget *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariableInputWidget(theRandomVariableIW)
{   
    //
    // create layout for all qLineEdits
    //

    QGridLayout *layout = new QGridLayout();

    //
    // for each QlineEdit create a label, the Qline Edit and set some defaults
    //

    QLabel *label1 = new QLabel();
    label1->setText(QString("Algorithm: "));
    layout->addWidget(label1, 0, 0);
    theAlgorithm = new QLineEdit();
    theAlgorithm->setText("Newmark");
    layout->addWidget(theAlgorithm, 0, 1);

    QLabel *label2 = new QLabel();
    label2->setText(QString("Integration: "));
    layout->addWidget(label2, 1, 0);
    theIntegration = new QLineEdit();
    theIntegration->setText("Newmark 0.5 0.25");
    layout->addWidget(theIntegration, 1, 1);

    QLabel *label3 = new QLabel();
    label3->setText(QString("ConvergenceTest: "));
    layout->addWidget(label3, 2, 0);
    theConvergenceTest = new QLineEdit();
    theConvergenceTest->setText("NormUnbalance");
    layout->addWidget(theConvergenceTest, 2, 1);

    QLabel *label4 = new QLabel();
    label4->setText(QString("Tolerance: "));
    layout->addWidget(label4, 3, 0);
    theTolerance = new QLineEdit();
    theTolerance->setText("0.01");
    layout->addWidget(theTolerance, 3, 1);

    // set the widgets layout
    this->setLayout(layout);

    this->setMinimumWidth(200);
    this->setMaximumWidth(400);
}

InputWidgetOpenSeesAnalysis::~InputWidgetOpenSeesAnalysis() {

}


void InputWidgetOpenSeesAnalysis::clear(void) {
    theIntegration->setText("Newmark 0.5 0.25");
    theAlgorithm->setText("Newmark");
    theConvergenceTest->setText("NormUnbalance");
    theTolerance->setText("0.01");
}

bool
InputWidgetOpenSeesAnalysis::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["type"]="OpenSeesAnalysis";
    jsonObject["integration"]=theIntegration->text();
    jsonObject["algorithm"]=theAlgorithm->text();
    jsonObject["tolerance"]=theTolerance->text();
    jsonObject["convergenceTest"]=theConvergenceTest->text();

    return result;
}


bool
InputWidgetOpenSeesAnalysis::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();


    if (jsonObject.contains("integration") && jsonObject.contains("algorithm")
            && jsonObject.contains("convergenceTest") && jsonObject.contains("tolerance")) {

        theTolerance->setText(jsonObject["tolerance"].toString());
        theAlgorithm->setText(jsonObject["algorithm"].toString());
        theConvergenceTest->setText(jsonObject["convergenceTest"].toString());
        theIntegration->setText(jsonObject["integration"].toString());

    } else {
        emit sendErrorMessage("ERROR: InputWidgetOpenSeesAnalysis - no \"integration\" ,\"convergenceTest\" or \"algorithm\" data");
        return false;
    }


    // should never get here .. if do my logic is screwy and need to return a false
    emit sendErrorMessage("ERROR - faulty logic - contact code developers");
    return result;
}



bool
InputWidgetOpenSeesAnalysis::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "OpenSees-Simulation";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return true;
}


bool
InputWidgetOpenSeesAnalysis::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    return true;
}


