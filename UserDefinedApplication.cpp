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

#include "UserDefinedApplication.h"
#include <QPushButton>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
#include <QGridLayout>
#include <QSpacerItem>

#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

UserDefinedApplication::UserDefinedApplication(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{
    QGridLayout *layout = new QGridLayout();

    QLabel *labelA = new QLabel();
    labelA->setText("Application Name");
    application = new QLineEdit;

    QLabel *labelF = new QLabel();
    labelF->setText("Application Args File");
    file = new QLineEdit;

    QPushButton *chooseFile = new QPushButton();
    chooseFile->setText(tr("Choose"));
    connect(chooseFile,SIGNAL(clicked()),this,SLOT(chooseFileName()));

    layout->addWidget(labelA,0,0);
    layout->addWidget(application,0,1);
    layout->addWidget(labelF,1,0);
    layout->addWidget(file,1,1);
    layout->addWidget(chooseFile,1,2);

    //QSpacerItem *spacer = new QSpacerItem();
    //layout->addItem(spacer,2,3);
    layout->setRowStretch(3,1);
    //layout->setColumnStretch(3,1);

    this->setLayout(layout);

}

UserDefinedApplication::~UserDefinedApplication()
{

}


void UserDefinedApplication::clear(void)
{

}



bool
UserDefinedApplication::outputToJSON(QJsonObject &jsonObject)
{
    // just need to send the class type here.. type needed in object in case user screws up
    jsonObject["type"]="UserDefinedApplication";

    return true;
}


bool
UserDefinedApplication::inputFromJSON(QJsonObject &jsonObject)
{

    return true;
}


bool
UserDefinedApplication::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = applicationName;
    QJsonObject dataObj;
    QString fileName = file->text();
    QFileInfo fileInfo(fileName);
    QDir copyPath(copyFilePath);

    dataObj["fileName"]= fileInfo.fileName();
    dataObj["filePath"]=fileInfo.path();
    dataObj["copyFilePath"]=copyPath.absolutePath();

    jsonObject["ApplicationData"] = dataObj;

    return true;
}
bool
UserDefinedApplication::inputAppDataFromJSON(QJsonObject &jsonObject) {

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

        file->setText(QDir(filePath).filePath(fileName));

        if (dataObject.contains("applicationName")) {
            QJsonValue theName = dataObject["applicationName"];
            applicationName = theName.toString();
	    application->setText(applicationName);
        } else
            return false;


        if (dataObject.contains("copyFilePath")) {
            QJsonValue theName = dataObject["copyFilePath"];
            copyFilePath = theName.toString();
        } else
            return false;


    } else {
        return false;
    }
}



void
UserDefinedApplication::chooseFileName(void) {
    fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    file->setText(fileName);
}


 bool
 UserDefinedApplication::copyFiles(QString &dirName) {

   copyFilePath = dirName;
   return true;
 }

