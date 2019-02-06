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
#include <RandomVariablesContainer.h>


#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>


InputWidgetOpenSeesAnalysis::InputWidgetOpenSeesAnalysis(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
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
    theAlgorithm->setText("Newton");
    theAlgorithm->setToolTip(tr("Nonlinear Solution Algorithm"));
    layout->addWidget(theAlgorithm, 0, 1);

    QLabel *label2 = new QLabel();
    label2->setText(QString("Integration: "));
    layout->addWidget(label2, 1, 0);
    theIntegration = new QLineEdit();
    theIntegration->setText("Newmark 0.5 0.25");
    theIntegration->setToolTip(tr("Command specifying integration scheme"));
    layout->addWidget(theIntegration, 1, 1);

    QLabel *label3 = new QLabel();
    label3->setText(QString("ConvergenceTest: "));
    layout->addWidget(label3, 2, 0);
    theConvergenceTest = new QLineEdit();
    theConvergenceTest->setText("NormUnbalance");
    theConvergenceTest->setToolTip(tr("Convergence test used in script, NormUnbalance, NormDispIncr, NormEnergy are options"));
    layout->addWidget(theConvergenceTest, 2, 1);

    QLabel *label4 = new QLabel();
    label4->setText(QString("Tolerance: "));
    layout->addWidget(label4, 3, 0);
    theTolerance = new QLineEdit();
    theTolerance->setToolTip(tr("2Norm on the unbalance used in convergence check"));
    theTolerance->setText("0.01");
    layout->addWidget(theTolerance, 3, 1);

    QLabel *label5 = new QLabel();
    label5->setText(QString("Damping Ratio: "));
    layout->addWidget(label5, 4, 0);
    dampingRatio = new QLineEdit();
    dampingRatio->setText("0.02");
    dampingRatio->setToolTip(tr("Damp ratio, 0.02 = 2% damping"));
    layout->addWidget(dampingRatio, 4, 1);

    QLabel *labelFile = new QLabel();
    labelFile->setText("Analysis Script: ");
    file = new QLineEdit;
    file->setToolTip(tr("User provided analysis script, replaces OpenSees default"));
    layout->addWidget(labelFile, 5, 0);
    layout->addWidget(file, 5, 1);

    QPushButton *chooseFile = new QPushButton();
    chooseFile->setText(tr("Choose"));
    layout->addWidget(chooseFile, 5, 2);

    connect(dampingRatio,SIGNAL(editingFinished()), this, SLOT(dampingEditingFinished()));
    connect(theTolerance,SIGNAL(editingFinished()), this, SLOT(toleranceEditingFinished()));
    connect(chooseFile, SIGNAL(clicked(bool)), this, SLOT(chooseFileName()));

    QWidget *dummy = new QWidget();
    layout->addWidget(dummy,6,0);
    layout->setRowStretch(6,1);

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
    dampingRatio->setText("0.02");
    file->setText("");
}

bool
InputWidgetOpenSeesAnalysis::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    jsonObject["Application"] = "OpenSees-Simulation";
    jsonObject["integration"]=theIntegration->text();
    jsonObject["algorithm"]=theAlgorithm->text();
    jsonObject["convergenceTest"]=theConvergenceTest->text();

    QString tolText = theTolerance->text();
    bool ok;
    double tolDouble = tolText.QString::toDouble(&ok);
    if (ok == true)
        jsonObject["tolerance"]=tolDouble;
    else
        jsonObject["tolerance"]= QString("RV.") + tolText;

    QString dampText = dampingRatio->text();
    double dampDouble = dampText.QString::toDouble(&ok);
    if (ok == true)
        jsonObject["dampingRatio"]=dampDouble;
    else
        jsonObject["dampingRatio"]= QString("RV.") + dampText;

    if (!file->text().isEmpty() && !file->text().isNull()) {
        QFileInfo fileInfo(file->text());
        jsonObject["fileName"]= fileInfo.fileName();
        jsonObject["filePath"]=fileInfo.path();
    }

    return result;
}


bool
InputWidgetOpenSeesAnalysis::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    this->clear();

    if (jsonObject.contains("integration") && jsonObject.contains("algorithm")
	&& jsonObject.contains("convergenceTest")) {

        theAlgorithm->setText(jsonObject["algorithm"].toString());
        theConvergenceTest->setText(jsonObject["convergenceTest"].toString());
        theIntegration->setText(jsonObject["integration"].toString());

    } else {
        emit sendErrorMessage("ERROR: InputWidgetOpenSeesAnalysis - no \"integration\" ,\"convergenceTest\" or \"algorithm\" data");
        return false;
    }

    if (jsonObject.contains("tolerance")) {
        QJsonValue theValue = jsonObject["tolerance"];
        if (theValue.isString()) {
            QString text = theValue.toString();
            text.remove(0,3); // remove RV.
           theTolerance->setText(text);
       } else if (theValue.isDouble())
            theTolerance->setText(QString::number(theValue.toDouble()));
    } else {
      emit sendErrorMessage("ERROR: InputWidgetOpenSeesAnalysis - no \"tolerance\" data");
        return false;
    }

    if (jsonObject.contains("dampingRatio")) {
        QJsonValue theValue = jsonObject["dampingRatio"];
        if (theValue.isString()) {
            QString text = theValue.toString();
            text.remove(0,3); // remove RV.
            dampingRatio->setText(text);
       } else if (theValue.isDouble())
            dampingRatio->setText(QString::number(theValue.toDouble()));
    } else {
      emit sendErrorMessage("ERROR: InputWidgetOpenSeesAnalysis - no \"dampingRatio\" data");
        return false;
    }

    if (jsonObject.contains("fileName")) {
        QJsonValue theName = jsonObject["fileName"];
        QString fileName = theName.toString();
        if (jsonObject.contains("filePath")) {
            QJsonValue theName = jsonObject["filePath"];
            QString filePath = theName.toString();
            file->setText(QDir(filePath).filePath(fileName));
        } else
            return false;
    }


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


void
InputWidgetOpenSeesAnalysis::chooseFileName(void) {
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    file->setText(fileName);
}

bool
InputWidgetOpenSeesAnalysis::copyFiles(QString &dirName) {
    if (file->text().isNull() || file->text().isEmpty()) {
        return true;
    }
    if  (this->copyFile(file->text(), dirName) ==  false) {
        emit sendErrorMessage(QString("ERROR: OpenSees Analysis copyFiles: failed to copy file: ") +file->text());
        return false;
    }
}

// need to check if a random variable
void InputWidgetOpenSeesAnalysis::dampingEditingFinished() {
    QString text = dampingRatio->text();
    bool ok;
    double dampDouble = text.QString::toDouble(&ok);

    if (ok == false) {
        qDebug() << text << " " << lastDampingRatio;

        if (text != lastDampingRatio) {
            QStringList rvs;
            rvs.append(text);
            rvs.append("0.02");
            theRandomVariablesContainer->addConstantRVs(rvs);
            lastDampingRatio = text;
        }
    }
}

void InputWidgetOpenSeesAnalysis::toleranceEditingFinished() {
    QString text = theTolerance->text();
    bool ok;
    double tolDouble = text.QString::toDouble(&ok);

    if (ok == false) {
        if (text != lastTolerance) {
            QStringList rvs;
            rvs.append(text);
            rvs.append("0.02");
            theRandomVariablesContainer->addConstantRVs(rvs);
            lastTolerance = text;
        }
    }
}
