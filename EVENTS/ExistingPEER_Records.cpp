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

// Written: fmckenna

#include "ExistingPEER_Records.h"
#include <InputWidgetExistingEvent.h>
#include <RandomVariablesContainer.h>

#include <QSpinBox>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <sectiontitle.h>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QScrollArea>
#include <LineEditRV.h>

PeerRecord::PeerRecord(RandomVariablesContainer *theRV_IW, QWidget *parent)
:SimCenterWidget(parent), theRandVariableIW(theRV_IW)
{
   QHBoxLayout *layout = new QHBoxLayout();
   button = new QRadioButton();

   QLabel *labelFile = new QLabel();
   labelFile->setText("File");
   file = new QLineEdit;

   QLabel *labelDirn = new QLabel();
   labelDirn->setText("DOF");
   dirn = new QSpinBox();
   dirn->setMaximum(3);
   dirn->setMinimum(1);

   QPushButton *chooseFile = new QPushButton();
   chooseFile->setText(tr("Choose"));
   connect(chooseFile,SIGNAL(clicked()),this,SLOT(chooseFileName()));

   QLabel *labelFactor = new QLabel(tr("Factor"));
   factor = new LineEditRV(theRV_IW);
   factor->setText(QString("1.0"));

   layout->addWidget(button);
   layout->addWidget(labelFile);
   layout->addWidget(file, 1.0);
   layout->addWidget(chooseFile);
   layout->addWidget(labelDirn);
   layout->addWidget(dirn);
   layout->addWidget(labelFactor);
   layout->addWidget(factor);

  // layout->addStretch();
   this->setLayout(layout);
   connect(button,SIGNAL(clicked(bool)),this,SLOT(onRemoveRecord(bool)));
}

PeerRecord::~PeerRecord()
{

}


void
PeerRecord::chooseFileName(void) {
    QString fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    file->setText(fileName1);
    QFileInfo fileInfo(file->text());
}


void
PeerRecord::onRemoveRecord(bool value) {
    if (value == true)
        emit removeRecord();
    return;
}

bool
PeerRecord::outputToJSON(QJsonObject &jsonObject) {
    QFileInfo fileInfo(file->text());
    jsonObject["fileName"]= fileInfo.fileName();
    jsonObject["filePath"]=fileInfo.path();
    jsonObject["dirn"]=dirn->value();

    return factor->outputToJSON(jsonObject, QString("factor"));
}

bool
PeerRecord::inputFromJSON(QJsonObject &jsonObject) {

    QString fileName;
    QString filePath;

    if (jsonObject.contains("fileName")) {
        QJsonValue theValue = jsonObject["fileName"];
        fileName = theValue.toString();
    } else
        return false;

    if (jsonObject.contains("filePath")) {
        QJsonValue theValue = jsonObject["filePath"];
        filePath = theValue.toString();
    } else
        return false;

    file->setText(QDir(filePath).filePath(fileName));

    if (jsonObject.contains("dirn")) {
        QJsonValue theValue = jsonObject["dirn"];
        dirn->setValue(theValue.toInt());
    } else
        return false;

    return factor->inputFromJSON(jsonObject, QString("factor"));
}


PeerEvent::PeerEvent(RandomVariablesContainer *theRV_IW, QWidget *parent)
    :SimCenterWidget(parent), theRandVariableIW(theRV_IW)
{
   QGroupBox *boxWidget = new QGroupBox;
   QHBoxLayout *groupBoxLayout = new QHBoxLayout();

   QHBoxLayout *layout = new QHBoxLayout();
   button = new QRadioButton();
   theName = new QLineEdit();
   //theName->setReadOnly(true);

   layout->addWidget(button);
   layout->addWidget(theName);

   // QVBox Holding Peer Records, could be multiple
    recordLayout = new QVBoxLayout();

   // QHBoxLayout *plusLayout = new QHBoxLayout();
    QPushButton *plus = new QPushButton();
    plus->setText("+");
    QPushButton *remove = new QPushButton();
    remove->setText("-");

    layout->addWidget(plus);
    layout->addWidget(remove);
    connect(plus, SIGNAL(clicked(bool)), this, SLOT(onAddRecord(bool)));
    connect(remove, SIGNAL(clicked(bool)), this, SLOT(onRemoveRecord(bool)));


   PeerRecord *theRecord = new PeerRecord(theRandVariableIW);
   recordLayout->addWidget(theRecord);
   theRecords.append(theRecord);
  // connect(theRecord,SIGNAL(removeRecord()), this, SLOT(onRemoveRecord()));

   layout->addLayout(recordLayout, 1.0);


   //this->setLayout(layout)
   boxWidget->setLayout(layout);
   groupBoxLayout->addWidget(boxWidget);
   this->setLayout(groupBoxLayout);
}

PeerEvent::~PeerEvent()
{

}

bool
PeerEvent::outputToJSON(QJsonObject &jsonObject) {
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "PeerEvent";
    jsonObject["name"]=theName->text();

    bool result = true;
    QJsonArray theArray;
    for (int i = 0; i <theRecords.size(); ++i) {
        QJsonObject theObj;
        if (theRecords.at(i)->outputToJSON(theObj)) {
            theArray.append(theObj);

        } else {
            qDebug() << "OUTPUT PEER EVENT FAILED" << this->theName->text();
            result = false;
        }
    }
    jsonObject["Records"]=theArray;
    return result;
    return true;
}

bool
PeerEvent::inputFromJSON(QJsonObject &jsonObject) {

    if (jsonObject.contains("name")) {
        QJsonValue theValue = jsonObject["name"];
        theName->setText(theValue.toString());
    } else
        return false;

    // clear out current list of Records
    for (int i = 0; i <theRecords.size(); ++i) {
      PeerRecord *theRecord = theRecords.at(i);
      recordLayout->removeWidget(theRecord);
      delete theRecord;
    }
    theRecords.clear();

    //
    // go get InputWidgetExistingEvents array from the JSON object
    // for each object in array:
    //    1)get it'is type,
    //    2)instantiate one
    //    4) get it to input itself
    //    5) finally add it to layout
    //

    // get array

    if (jsonObject.contains("Records"))
        if (jsonObject["Records"].isArray()) {

            QJsonArray rvArray = jsonObject["Records"].toArray();

            // foreach object in array
            foreach (const QJsonValue &rvValue, rvArray) {

                // create Record, read it and add to widget and QVector

                QJsonObject jsonObject = rvValue.toObject();
                PeerRecord *theRecord = new PeerRecord(theRandVariableIW);

                if (theRecord->inputFromJSON(jsonObject)) { // this method is where type is set
                    theRecords.append(theRecord);
                    recordLayout->addWidget(theRecord);
                } else {
                    delete theRecord;
                    return false;
                }
            }
        }

    return true;
}

void
PeerEvent::onAddRecord(bool) {
    PeerRecord *theRecord = new PeerRecord(theRandVariableIW);
    recordLayout->addWidget(theRecord);
    theRecords.append(theRecord);
}

void
PeerEvent::onRemoveRecord(bool) {
    // find the ones selected & remove them
    int numRecords = theRecords.size();
    for (int i = numRecords-1; i >= 0; i--) {
      PeerRecord *theRecord = theRecords.at(i);
      if (theRecord->button->isChecked()) {
          theRecord->close();
          recordLayout->removeWidget(theRecord);
          theRecords.remove(i);
          theRecord->setParent(0);
          delete theRecord;
      }
    }
}

ExistingPEER_Records::ExistingPEER_Records(RandomVariablesContainer *theRV_IW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandVariableIW(theRV_IW)
{
    verticalLayout = new QVBoxLayout();

    // title, add & remove button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("List of PEER Events"));
    title->setMinimumWidth(250);


    QPushButton *addEvent = new QPushButton();
    addEvent->setMinimumWidth(75);
    addEvent->setMaximumWidth(75);
    addEvent->setText(tr("Add"));
    //    connect(addEvent,SIGNAL(clicked()),this,SLOT(addInputWidgetPeerEvent()));

    QPushButton *removeEvent = new QPushButton();
    removeEvent->setMinimumWidth(75);
    removeEvent->setMaximumWidth(75);
    removeEvent->setText(tr("Remove"));
    //    connect(removeEvent,SIGNAL(clicked()),this,SLOT(removeInputWidgetPeerEvent()));


    QPushButton *loadDirectory = new QPushButton();
    loadDirectory->setMinimumWidth(150);
    loadDirectory->setMaximumWidth(150);
    loadDirectory->setText(tr("Load Directory"));

    titleLayout->addWidget(title);
    titleLayout->addSpacing(50);
    titleLayout->addWidget(addEvent);
    titleLayout->addSpacing(20);
    titleLayout->addWidget(removeEvent);
    titleLayout->addSpacing(50);
    titleLayout->addWidget(loadDirectory);
    titleLayout->addStretch();

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *eventsWidget = new QWidget;

    eventLayout = new QVBoxLayout();
    eventLayout->addStretch();
    eventsWidget->setLayout(eventLayout);
    sa->setWidget(eventsWidget);


    verticalLayout->addLayout(titleLayout);
    verticalLayout->addWidget(sa);
    //verticalLayout->addStretch();

    this->setLayout(verticalLayout);


    connect(addEvent, SIGNAL(pressed()), this, SLOT(addEvent()));
    connect(removeEvent, SIGNAL(pressed()), this, SLOT(removeEvents()));
    connect(loadDirectory, SIGNAL(pressed()), this, SLOT(loadEventsFromDir()));
}


ExistingPEER_Records::~ExistingPEER_Records()
{

}


void ExistingPEER_Records::addEvent(void)
{
   PeerEvent *theEvent = new PeerEvent(theRandVariableIW);
   theEvents.append(theEvent);
   eventLayout->insertWidget(eventLayout->count()-1, theEvent);
   //connect(this,SLOT(InputWidgetExistingEventErrorMessage(QString)), theEvent, SIGNAL(sendErrorMessage(QString)));
}

void ExistingPEER_Records::loadEventsFromDir(void) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    this->clear();

    QDir directory(dir);
    QString recordsTxt(directory.filePath("Records.txt"));
    QFileInfo checkFile(recordsTxt);
    if (checkFile.exists() && checkFile.isFile()) {
        QFile file(recordsTxt);
        if (!file.open(QIODevice::ReadOnly)) {
                qDebug() << file.errorString();
                return;
            }

            QStringList wordList;
            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                QByteArrayList lineList = line.split(',');

                if (lineList.length() >= 2) {

                    QString fileName = lineList.at(0);
                    QString factor = lineList.at(1);
                    factor.remove(QRegExp("[\\n\\t\\r]"));

                    QFileInfo checkName(directory.filePath(fileName));
                    if (checkName.exists() && checkName.isFile()) {


                        PeerEvent *theEvent = new PeerEvent(theRandVariableIW);

                        QString name(fileName);
                        name.chop(4); // remove .AT2
                        if (name.contains("RSN")) {
                            name.remove(0,3); // remove RSN
                            int index = name.indexOf(QString("_"));
                            if (index != -1)
                                name=name.left(index);
                        }

                        theEvent->theName->setText(name);
                        PeerRecord *theRecord = theEvent->theRecords.at(0);
                        if (theRecord != NULL) {
                            theRecord->file->setText(directory.filePath(fileName));
                            theRecord->factor->setText(factor);
                        }

                        theEvents.append(theEvent);
                        eventLayout->insertWidget(eventLayout->count()-1, theEvent);
                    } else {
                        qDebug() << "Existing PEER_Records: load directory file " << fileName << " does not exist";
                    }
                }
            }

            file.close();

    } else {

        QStringList fileList= directory.entryList(QStringList() << "*.AT2",QDir::Files);
        foreach(QString fileName, fileList) {

            PeerEvent *theEvent = new PeerEvent(theRandVariableIW);
            QString name = fileName;
            name.chop(4); // remove .AT2
            if (name.contains("RSN")) {
                name.remove(0,3); // remove RSN
                int index = name.indexOf(QString("_"));
                if (index != -1)
                    name=name.left(index);
            }

            theEvent->theName->setText(name);
            PeerRecord *theRecord = theEvent->theRecords.at(0);
            if (theRecord != NULL) {
                theRecord->file->setText(directory.filePath(fileName));
            }
            theEvents.append(theEvent);
            eventLayout->insertWidget(eventLayout->count()-1, theEvent);
        }
    }

}

void ExistingPEER_Records::removeEvents(void)
{
    // find the ones selected & remove them
    int numInputWidgetExistingEvents = theEvents.size();
    for (int i = numInputWidgetExistingEvents-1; i >= 0; i--) {
      PeerEvent *theEvent = theEvents.at(i);
      if (theEvent->button->isChecked()) {
          theEvent->close();
          eventLayout->removeWidget(theEvent);
          theEvents.remove(i);
          theEvent->setParent(0);
          delete theEvent;
      }
    }
}


void
ExistingPEER_Records::clear(void)
{
  // loop over random variables, removing from layout & deleting
  for (int i = 0; i <theEvents.size(); ++i) {
    PeerEvent *theEvent = theEvents.at(i);
    eventLayout->removeWidget(theEvent);
    delete theEvent;
  }
  theEvents.clear();
}


bool
ExistingPEER_Records::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "ExistingPEER_Events";

    bool result = true;
    QJsonArray theArray;
    for (int i = 0; i <theEvents.size(); ++i) {
        QJsonObject rv;
        if (theEvents.at(i)->outputToJSON(rv)) {
            theArray.append(rv);

        } else {
            qDebug() << "OUTPUT FAILED" << theEvents.at(i)->theName->text();
            result = false;
        }
    }
    jsonObject["Events"]=theArray;
    return result;
}

bool
ExistingPEER_Records::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    // clean out current list
    this->clear();

    // get array
    if (jsonObject.contains("Events")) {
        if (jsonObject["Events"].isArray()) {

            QJsonArray eventArray = jsonObject["Events"].toArray();

            // foreach object in array
            foreach (const QJsonValue &eventValue, eventArray) {

                // get data, create an event, read it and then add to layout

                QJsonObject eventObject = eventValue.toObject();
                PeerEvent *theEvent = new PeerEvent(theRandVariableIW);

                if (theEvent->inputFromJSON(eventObject)) { // this method is where type is set
                    theEvents.append(theEvent);
                    eventLayout->insertWidget(eventLayout->count()-1, theEvent);
                } else {
                    delete theEvent;
                    result = false;
                }
            }
        }
    }

  return result;
}


bool
ExistingPEER_Records::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["EventClassification"]="Earthquake";
    jsonObject["Application"] = "ExistingPEER_Events";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    return true;
}

bool
ExistingPEER_Records::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}

bool 
ExistingPEER_Records::copyFiles(QString &dirName) {

    for (int i=0; i<theEvents.size(); i++) {
        PeerEvent *theEvent = theEvents.at(i);
        for (int j=0; j<theEvent->theRecords.size(); j++) {
            QString fileName = theEvents.at(i)->theRecords.at(j)->file->text();
            if (this->copyFile(fileName, dirName) ==  false) {
                emit errorMessage(QString("ERROR: copyFiles: failed to copy") + theEvents.at(i)->theRecords.at(j)->file->text());
                return false;
            }
        }
    }
    return true;
}

void
ExistingPEER_Records::errorMessage(QString message){
    emit sendErrorMessage(message);
}

