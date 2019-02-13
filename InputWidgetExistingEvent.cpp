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

#include "InputWidgetExistingEvent.h"
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
#include <QFileInfo>

#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

InputWidgetExistingEvent::InputWidgetExistingEvent(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{
    femSpecific = 0;

    layout = new QVBoxLayout();
    QHBoxLayout *fileName1Layout = new QHBoxLayout();

    QLabel *label1 = new QLabel();
    label1->setText("File");

    file1 = new QLineEdit;
    QPushButton *chooseFile1 = new QPushButton();
    chooseFile1->setText(tr("Choose"));
    connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));

    fileName1Layout->addWidget(label1);
    fileName1Layout->addWidget(file1);
    fileName1Layout->addWidget(chooseFile1);
    fileName1Layout->addStretch();

    fileName1Layout->setSpacing(10);
    fileName1Layout->setMargin(0);

    layout->addLayout(fileName1Layout);
    layout->setSpacing(10);
    layout->setMargin(0);
    layout->addStretch();

    layout->setMargin(0);
    layout->addStretch();

    this->setLayout(layout);

}

InputWidgetExistingEvent::~InputWidgetExistingEvent()
{

}


void InputWidgetExistingEvent::clear(void)
{

}



bool
InputWidgetExistingEvent::outputToJSON(QJsonObject &jsonObject)
{
    // just need to send the class type here.. type needed in object in case user screws up
    jsonObject["type"]="SimCenterEvent";
    jsonObject["EventClassification"]="Earthquake";
    return true;
}


bool
InputWidgetExistingEvent::inputFromJSON(QJsonObject &jsonObject)
{
    this->clear();
    return true;
}


bool
InputWidgetExistingEvent::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["EventClassification"]="Earthquake";
    jsonObject["Application"] = "SimCenterEvent";
    QJsonObject dataObj;
    QString fileName = file1->text();
    QFileInfo fileInfo(fileName);

    dataObj["fileName"]= fileInfo.fileName();
    dataObj["filePath"]=fileInfo.path();

    jsonObject["ApplicationData"] = dataObj;

    return true;
}
bool
InputWidgetExistingEvent::inputAppDataFromJSON(QJsonObject &jsonObject) {

    if (jsonObject.contains("ApplicationData")) {

        QJsonValue theName = jsonObject["ApplicationData"];
        QJsonObject dataObject = theName.toObject();

        QString fileName;
        QString filePath;

        if (dataObject.contains("fileName")) {
            QJsonValue theName = dataObject["fileName"];
            fileName = theName.toString();
        } else
            return false;

        if (dataObject.contains("filePath")) {
            QJsonValue theName = dataObject["filePath"];
            filePath = theName.toString();
        } else
            return false;

        file1->setText(QDir(filePath).filePath(fileName));

    } else
        return false;

    return true;
}




int
InputWidgetExistingEvent::setFilename1(QString name1){

    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }

    theRandomVariablesContainer->removeRandomVariables(names);

    // set file name & ebtry in qLine edit

    fileName1 = name1;
    file1->setText(name1);

    return 0;
}

void
InputWidgetExistingEvent::chooseFileName1(void) {
    fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    int ok = this->setFilename1(fileName1);
}


QString InputWidgetExistingEvent::getMainInput() {
    return fileName1;
}

 bool
 InputWidgetExistingEvent::copyFiles(QString &destDir) {

     QString fileName = file1->text();
     if (fileName.isEmpty()) {
         return false;
     }
     return this->copyFile(fileName, destDir);
 }

