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

#include "UserDefinedEDP.h"
#include <EDP.h>

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
#include <fstream>
#include <iostream>
using namespace std;
#include <QGridLayout>

#include <OpenSeesParser.h>
#include <RandomVariablesContainer.h>

//#include <InputWidgetParameters.h>

UserDefinedEDP::UserDefinedEDP(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{

    QVBoxLayout *layout = new QVBoxLayout();


   // label1->setText("Processing Script");

   // QHBoxLayout *fileName1Layout = new QHBoxLayout();
    QGridLayout *scriptLayout = new QGridLayout();

    // add stuff to enter Additional INput
    QLabel *labelAI = new QLabel("Additional Input");
    additionalInputLE = new QLineEdit;
    QPushButton *chooseAdditionalInputButton = new QPushButton();
    chooseAdditionalInputButton->setText(tr("Choose"));
    connect(chooseAdditionalInputButton,SIGNAL(clicked()),this,SLOT(chooseAdditionalInput()));
    scriptLayout->addWidget(labelAI,0,0);
    scriptLayout->addWidget(additionalInputLE,0,2);
    scriptLayout->addWidget(chooseAdditionalInputButton, 0, 4);

    // add stuff to enter processing script
    QLabel *labelPS = new QLabel("Processing Script");
    processingScriptLE = new QLineEdit;
    QPushButton *chooseProcessingScriptButton = new QPushButton();
    chooseProcessingScriptButton->setText(tr("Choose"));
    connect(chooseProcessingScriptButton,SIGNAL(clicked()),this,SLOT(chooseProcessingScript()));
    scriptLayout->addWidget(labelPS,1,0);
    scriptLayout->addWidget(processingScriptLE,1,2);
    scriptLayout->addWidget(chooseProcessingScriptButton, 1, 4);

    QSpacerItem *spacer3 = new QSpacerItem(30,10);
    QSpacerItem *spacer4 = new QSpacerItem(10,10);
    scriptLayout->addItem(spacer3,0,1);
    scriptLayout->addItem(spacer4,0,3);
    scriptLayout->setColumnStretch(5,1);
    scriptLayout->setColumnStretch(2,1);

    scriptLayout->setHorizontalSpacing(5);
    scriptLayout->setVerticalSpacing(5);

    layout->addLayout(scriptLayout);
    QSpacerItem *spacer = new QSpacerItem(20,10);
    layout->addSpacerItem(spacer);


    //layout->setColumnStretch(3,1);

    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Response Parameters"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);

    QPushButton *addEDP = new QPushButton();
    addEDP->setMinimumWidth(75);
    addEDP->setMaximumWidth(75);
    addEDP->setText(tr("Add"));
    connect(addEDP,SIGNAL(clicked()),this,SLOT(addEDP()));

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addEDP);
    titleLayout->addItem(spacer2);

    /*
    QPushButton *removeEDP = new QPushButton();
    removeEDP->setMinimumWidth(75);
    removeEDP->setMaximumWidth(75);
    removeEDP->setText(tr("Remove"));
    connect(removeEDP,SIGNAL(clicked()),this,SLOT(removeEDP()));
      titleLayout->addWidget(removeEDP);
*/

    titleLayout->addStretch();

    //layout->addWidget(titleLayout,1,0,3,0);
    layout->addLayout(titleLayout);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    edp = new QFrame();
    edp->setFrameShape(QFrame::NoFrame);
    edp->setLineWidth(0);

    edpLayout = new QVBoxLayout;

    //setLayout(layout);
    edp->setLayout(edpLayout);
    edpLayout->addStretch();
    //this->addEDP();

    sa->setWidget(edp);
    layout->addWidget(sa);
    //layout->addStretch(1.0);

    layout->setSpacing(0);
    layout->setMargin(0);

    this->setLayout(layout);
    /*
    QWidget *dummyR = new QWidget();
    layout->addWidget(dummyR,3,0);
    layout->setRowStretch(4,1);
    this->setLayout(layout);
    */

}

UserDefinedEDP::~UserDefinedEDP()
{

}


void
UserDefinedEDP::clear(void)
{
    additionalInputLE->setText("");
    processingScriptLE->setText("");

    int numEDPs = theEDPs.size();
    for (int i = numEDPs-1; i >= 0; i--) {
        EDP *theEDP = theEDPs.at(i);
        theEDP->close();
        edpLayout->removeWidget(theEDP);
        theEDPs.remove(i);
        delete theEDP;
    }
}



bool
UserDefinedEDP::outputToJSON(QJsonObject &jsonObject)
{
    // just need to send the class type here.. type needed in object in case user screws up
    jsonObject["type"]="UserDefinedEDP";

    QString fileName = processingScriptLE->text();
    if (!fileName.isEmpty()) {
        QFileInfo fileInfoPS(fileName);
        if (fileInfoPS.exists()) {

            jsonObject["fileNamePS"]= fileInfoPS.fileName();
            jsonObject["filePathPS"]=fileInfoPS.path();

        } else {
            emit sendErrorMessage("UserDefinedEDP :: Proceesing script does not exist");
        }
    } else {
       emit sendErrorMessage("UserDefinedEDP :: no processing script");
    }

    fileName = additionalInputLE->text();
    if (!fileName.isEmpty()) {
        QFileInfo fileInfoPS(fileName);
        if (fileInfoPS.exists()) {

            jsonObject["fileNameAI"]= fileInfoPS.fileName();
            jsonObject["filePathAI"]=fileInfoPS.path();

        } else {
            emit sendErrorMessage("UserDefinedEDP :: additional script does not exist");
        }
    }


    QJsonArray edpArray;
    int numEDPs = theEDPs.size();
    for (int i = 0; i < numEDPs; i++) {
        QJsonObject edpObject;
        EDP *theEDP = theEDPs.at(i);
        theEDP->outputToJSON(edpObject);
        edpArray.append(edpObject);
    }
    jsonObject["EDP"]=edpArray;

    return true;
}


bool
UserDefinedEDP::inputFromJSON(QJsonObject &jsonObject)
{
    //
    // retrieve filename and path, set the QLIne Edit
    //

    QString fileName;
    QString filePath;

    if (jsonObject.contains("fileNamePS")) {
        QJsonValue theName = jsonObject["fileNamePS"];
        fileName = theName.toString();

        if (jsonObject.contains("filePathPS")) {
            QJsonValue theName = jsonObject["filePathPS"];
            filePath = theName.toString();

            processingScriptLE->setText(QDir(filePath).filePath(fileName));

        } else {
           emit sendErrorMessage("UserDefinedEDP ::inputFromJSON  no filePathPS exists");
        }

    } else {
        emit sendErrorMessage("UserDefinedEDP ::inputFromJSON  no NamePS exists");
    }

    if (jsonObject.contains("fileNameAI")) {
        QJsonValue theName = jsonObject["fileNameAI"];
        fileName = theName.toString();

        if (jsonObject.contains("filePathAI")) {
            QJsonValue theName = jsonObject["filePathAI"];
            filePath = theName.toString();

            additionalInputLE->setText(QDir(filePath).filePath(fileName));

        } else {
         emit sendErrorMessage("UserDefinedEDP ::inputFromJSON  no filePathAI exists");
        }
    }



    if (jsonObject.contains("EDP")) {
        QJsonArray edpArray = jsonObject["EDP"].toArray();

        for (int i=0; i<edpArray.size(); i++) {
            QJsonObject edpData = edpArray.at(i).toObject();
            EDP *theEDP = new EDP();
            theEDPs.append(theEDP);
            theEDP->inputFromJSON(edpData);
            edpLayout->insertWidget(edpLayout->count()-1, theEDP);

            connect(theEDP, SIGNAL(removeEDP(EDP*)), this, SLOT(removeEDP(EDP*)));
        }
    }

    return true;
}


bool
UserDefinedEDP::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["Application"] = "UserDefinedEDP";
    QJsonObject dataObj;

    jsonObject["ApplicationData"] = dataObj;

    return true;
}
bool
UserDefinedEDP::inputAppDataFromJSON(QJsonObject &jsonObject) {

    //
    // from ApplicationData
    //

    this->clear();

}



int
UserDefinedEDP::setProcessingScript(QString name){

    // set file name & ebtry in qLine edit

    //filenameProcessingScript = name;
    this->clear();

    processingScriptLE->setText(name);

    //
    // parse first line of file & check or line for anything EDP:
    //

    ifstream inFile(name.toStdString());

    // read line 1 l & look for EDP
    // if EDP parse all strings seperated by :

    string line;
    string pattern("EDP:");

    int numLine = 0;
    while (numLine < 5) {
        getline(inFile, line);
        qDebug() << QString::fromStdString(line);

        std::size_t found = line.find(pattern);
        std::size_t start = 0;
        if (found!=std::string::npos) {
            start = found+4;
            string endMarker(";");
            while ((found = line.find(endMarker, start)) != std::string::npos) {
                std::string edpName = line.substr (start,found-start);
                QString name = QString::fromStdString(edpName);

                this->addEDP(name);
                start = found+1;
            }
            // add last
            std::string edpName = line.substr (start,line.length()-start-1);
            QString name = QString::fromStdString(edpName);
            qDebug() << QString("->") << name<< QString("<-") << start << " " << line.length();
            this->addEDP(name);
        }

        numLine++;
    }

    // close file
    inFile.close();

    /*
    OpenSeesParser theParser;
    varNamesAndValues = theParser.getVariables(fileName1);
    theRandomVariablesContainer->addConstantRVs(varNamesAndValues);
    */

    return 0;
}

void
UserDefinedEDP::chooseProcessingScript(void) {
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    int ok = this->setProcessingScript(fileName);
}

int
UserDefinedEDP::setAdditionalInput(QString filename) {
    additionalInputLE->setText(filename);
}

void
UserDefinedEDP::chooseAdditionalInput(void) {
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    int ok = this->setAdditionalInput(fileName);
}

 bool
 UserDefinedEDP::copyFiles(QString &dirName) {

     QString filename = processingScriptLE->text();
     if (!filename.isEmpty()) {

         if (this->copyFile(filename, dirName) ==  false) {
             emit sendErrorMessage(QString("ERROR: copyFiles: failed to copy") + filename);
             return false;
         }
     }

     filename = additionalInputLE->text();
     if (!filename.isEmpty()) {

         if (this->copyFile(filename, dirName) ==  false) {
             emit sendErrorMessage(QString("ERROR: copyFiles: failed to copy") + filename);
             return false;
         }
     }
 }


 void
 UserDefinedEDP::addEDP(void)
 {
     EDP *theEDP = new EDP();
     theEDPs.append(theEDP);
     // edpLayout->addWidget(theEDP);
     edpLayout->insertWidget(edpLayout->count()-1, theEDP);

     connect(theEDP, SIGNAL(removeEDP(EDP*)), this, SLOT(removeEDP(EDP*)));
 }

 void
 UserDefinedEDP::addEDP(QString &name)
 {
     EDP *theEDP = new EDP(&name, NULL);
     theEDPs.append(theEDP);
     edpLayout->insertWidget(edpLayout->count()-1, theEDP);

     connect(theEDP, SIGNAL(removeEDP(EDP*)), this, SLOT(removeEDP(EDP*)));
 }


void UserDefinedEDP::removeEDP(EDP *theEDPtoRemove)
{
    // find the ones selected & remove them
    int numEDPs = theEDPs.size();

     qDebug() << "removeEDP" << numEDPs;

    for (int i = numEDPs-1; i >= 0; i--) {
        EDP *theEDP = theEDPs.at(i);
        if (theEDP == theEDPtoRemove) {
            theEDP->close();
            edpLayout->removeWidget(theEDP);
            theEDPs.remove(i);
            //theEDP->setParent(0);
            delete theEDP;
            i=-1;
        }
    }
}

/*
int UserDefinedEDP::getNumEDP(void)
{
    return theEDPs.length();
}
*/
