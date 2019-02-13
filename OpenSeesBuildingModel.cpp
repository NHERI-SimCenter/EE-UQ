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

#include "OpenSeesBuildingModel.h"
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
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
#include <QGridLayout>

#include <OpenSeesParser.h>
#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

OpenSeesBuildingModel::OpenSeesBuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{
    femSpecific = 0;

    layout = new QGridLayout();

    QLabel *label1 = new QLabel();
    label1->setText("Input Script");

   // QHBoxLayout *fileName1Layout = new QHBoxLayout();
    file1 = new QLineEdit;
    QPushButton *chooseFile1 = new QPushButton();
    chooseFile1->setText(tr("Choose"));
    connect(chooseFile1,SIGNAL(clicked()),this,SLOT(chooseFileName1()));
    layout->addWidget(label1,0,0);
    layout->addWidget(file1,0,1);
    layout->addWidget(chooseFile1,0,2);

    QLabel *label2 = new QLabel();
    label2->setText("List of CLine Nodes:");
    nodes = new QLineEdit;

    layout->addWidget(label2,1,0);
    layout->addWidget(nodes,1,1);

    QLabel *label3 = new QLabel();
    label3->setText("Spatial Dimension:");
    ndm = new QLineEdit();
    ndm->setText("2");
    ndm->setMaximumWidth(50);
    layout->addWidget(label3,2,0);
    layout->addWidget(ndm,2,1);

    QWidget *dummyR = new QWidget();
    layout->addWidget(dummyR,3,0);
    layout->setRowStretch(4,1);
    //layout->setColumnStretch(3,1);

    this->setLayout(layout);

}

OpenSeesBuildingModel::~OpenSeesBuildingModel()
{
    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }

    theRandomVariablesContainer->removeRandomVariables(names);
}


void OpenSeesBuildingModel::clear(void)
{

}



bool
OpenSeesBuildingModel::outputToJSON(QJsonObject &jsonObject)
{
    // just need to send the class type here.. type needed in object in case user screws up
    jsonObject["type"]="OpenSeesInput";
    QJsonArray nodeTags;
    string nodeString = nodes->text().toStdString();
    string s1(nodeString); // this line is needed as nodeString cannot be passed directly to the line below!
    stringstream nodeStream(s1);
    int nodeTag;
    while (nodeStream >> nodeTag) {
            nodeTags.append(QJsonValue(nodeTag));
            if (nodeStream.peek() == ',')
                       nodeStream.ignore();
    }

    jsonObject["nodes"]=nodeTags;

    jsonObject["ndm"]=ndm->text().toInt();

    QJsonArray rvArray;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        QJsonObject rvObject;
        QString name = varNamesAndValues.at(i);
        rvObject["name"]=name;
        rvObject["value"]=QString("RV.")+name;
       rvArray.append(rvObject);
    }

    jsonObject["randomVar"]=rvArray;

    return true;
}


bool
OpenSeesBuildingModel::inputFromJSON(QJsonObject &jsonObject)
{
    varNamesAndValues.clear();

    this->clear();
    QString stringNodes;
    if (jsonObject.contains("nodes")) {
        QJsonArray nodeTags = jsonObject["nodes"].toArray();
        foreach (const QJsonValue & value, nodeTags) {
            int tag = value.toInt();
            stringNodes = stringNodes + " " +  QString::number(tag);
        }
    }

    nodes->setText(stringNodes);

    if (jsonObject.contains("randomVar")) {
        QJsonArray randomVars = jsonObject["randomVar"].toArray();
        foreach (const QJsonValue & value, randomVars) {
            QJsonObject theRV = value.toObject();
            QString name = theRV["name"].toString();
            QString zero = "0";
            varNamesAndValues.append(name);
            varNamesAndValues.append(zero);
        }
    }

    int theNDM = jsonObject["ndm"].toInt();
    qDebug() << "NDM: " << theNDM;

    ndm->setText(QString::number(theNDM));

    return true;
}


bool
OpenSeesBuildingModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "OpenSeesInput";
    QJsonObject dataObj;
    QString fileName = file1->text();
    QFileInfo fileInfo(fileName);

    dataObj["fileName"]= fileInfo.fileName();
    dataObj["filePath"]=fileInfo.path();

    jsonObject["ApplicationData"] = dataObj;

    return true;
}
bool
OpenSeesBuildingModel::inputAppDataFromJSON(QJsonObject &jsonObject) {

    //
    // from ApplicationData
    //

    if (jsonObject.contains("ApplicationData")) {
        QJsonObject dataObject = jsonObject["ApplicationData"].toObject();

        //
        // retrieve filename and path, set the QLIne Edit
        //

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

        //
        // get nodes and set QLineEdit
        //

        if (dataObject.contains("nodes")) {
            QJsonValue theName = dataObject["nodes"];
            nodes->setText(theName.toString());

        } else
            return false;

    } else {
        return false;
    }
}



int
OpenSeesBuildingModel::setFilename1(QString name1){

    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }

    theRandomVariablesContainer->removeRandomVariables(names);

    // set file name & ebtry in qLine edit

    fileName1 = name1;
    file1->setText(name1);

    //
    // parse file for random variables and add them
    //

    OpenSeesParser theParser;
    varNamesAndValues = theParser.getVariables(fileName1);

    theRandomVariablesContainer->addConstantRVs(varNamesAndValues);

    return 0;
}

void
OpenSeesBuildingModel::chooseFileName1(void) {
    fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    int ok = this->setFilename1(fileName1);
}

void
OpenSeesBuildingModel::specialCopyMainInput(QString fileName, QStringList varNames) {
    // if OpenSees or FEAP parse the file for the variables
    if (varNames.size() > 0) {
        OpenSeesParser theParser;
        theParser.writeFile(file1->text(), fileName, varNames);
    }
}


QString OpenSeesBuildingModel::getMainInput() {
    return fileName1;
}

 bool
 OpenSeesBuildingModel::copyFiles(QString &dirName) {

     QString fileName = file1->text();

     if (fileName.isEmpty()) {
         emit sendErrorMessage("OpenSeesInput - no file set");
         return false;
     }
     QFileInfo fileInfo(fileName);

     QString theFile = fileInfo.fileName();
     QString thePath = fileInfo.path();

     SimCenterAppWidget::copyPath(thePath, dirName, false);

     QStringList varNames = theRandomVariablesContainer->getRandomVariableNames();

     // now create special copy of original main script that handles the RV
     OpenSeesParser theParser;
     QString copiedFile = dirName + QDir::separator() + theFile;
     theParser.writeFile(fileName, copiedFile, varNames);
 }

