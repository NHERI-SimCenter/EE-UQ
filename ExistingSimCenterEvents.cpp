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

#include "ExistingSimCenterEvents.h"
#include <InputWidgetExistingEvent.h>
#include <RandomVariableInputWidget.h>

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


ExistingEvent::ExistingEvent(RandomVariableInputWidget *theRV_IW, QWidget *parent)
    :SimCenterWidget(parent), theRandVariableIW(theRV_IW)
{
   QHBoxLayout *layout = new QHBoxLayout();
   button = new QRadioButton();
   theName = new QLineEdit();
   theName->setReadOnly(true);

   QLabel *label = new QLabel();
   label->setText("File");
   file = new QLineEdit;

   QPushButton *chooseFile = new QPushButton();
   chooseFile->setText(tr("Choose"));
   connect(chooseFile,SIGNAL(clicked()),this,SLOT(chooseFileName()));

   QLabel *labelFactor = new QLabel(tr("Factor"));
   factor = new QLineEdit("1.0");
   lastFactor = "";
   connect(factor,SIGNAL(editingFinished()),this,SLOT(factorEditingFinished()));

   layout->addWidget(button);
   layout->addWidget(theName);
   layout->addWidget(label);
   layout->addWidget(file, 1.0);
   layout->addWidget(chooseFile);
   layout->addWidget(labelFactor);
   layout->addWidget(factor);

  // layout->addStretch();
   this->setLayout(layout);
}

ExistingEvent::~ExistingEvent()
{

}

// need to check if a random variable
void ExistingEvent::factorEditingFinished() {
    QString text = factor->text();
    bool ok;
    double factorDouble = text.QString::toDouble(&ok);

    if (ok == false) {
        if (text != lastFactor) {
            QStringList rvs;
            rvs.append(text);
            rvs.append("1.0");
            theRandVariableIW->addConstantRVs(rvs);
            lastFactor = text;
        }
    }
}

void
ExistingEvent::chooseFileName(void) {
    QString fileName1=QFileDialog::getOpenFileName(this,tr("Open File"),"C://", "All files (*.*)");
    file->setText(fileName1);
    QFileInfo fileInfo(file->text());
    QString baseName = fileInfo.baseName();
    theName->setText(baseName);
    theName->setReadOnly(false);
}


bool
ExistingEvent::outputToJSON(QJsonObject &jsonObject) {
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "ExistingEvent";
    QFileInfo fileInfo(file->text());
    jsonObject["fileName"]= fileInfo.fileName();
    jsonObject["filePath"]=fileInfo.path();
    jsonObject["name"]=theName->text();

    QString factorText = factor->text();
    bool ok;
    double factorDouble = factorText.QString::toDouble(&ok);
    if (ok == true)
        jsonObject["factor"]=factorDouble;
    else
        jsonObject["factor"]= QString("RV.") + factor->text();

    return true;
}

bool
ExistingEvent::inputFromJSON(QJsonObject &jsonObject) {
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

    if (jsonObject.contains("name")) {
        QJsonValue theValue = jsonObject["name"];
        theName->setText(theValue.toString());
    } else
        return false;

    if (jsonObject.contains("factor")) {
        QJsonValue theValue = jsonObject["factor"];
        if (theValue.isString()) {
            QString text = theValue.toString();
            text.remove(0,3); // remove RV.
           factor->setText(text);
       } else if (theValue.isDouble())
            factor->setText(QString::number(theValue.toDouble()));
    } else
        return false;

    return true;
}

ExistingSimCenterEvents::ExistingSimCenterEvents(RandomVariableInputWidget *theRV_IW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandVariableIW(theRV_IW)
{
    verticalLayout = new QVBoxLayout();

    // title, add & remove button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("List of SimCenter Events"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);

    QPushButton *addEvent = new QPushButton();
    addEvent->setMinimumWidth(75);
    addEvent->setMaximumWidth(75);
    addEvent->setText(tr("Add"));
    //   connect(addEvent,SIGNAL(clicked()),this,SLOT(addInputWidgetExistingEvent()));

    QPushButton *removeEvent = new QPushButton();
    removeEvent->setMinimumWidth(75);
    removeEvent->setMaximumWidth(75);
    removeEvent->setText(tr("Remove"));
    //    connect(removeEvent,SIGNAL(clicked()),this,SLOT(removeInputWidgetExistingEvent()));

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addEvent);
    titleLayout->addItem(spacer2);
    titleLayout->addWidget(removeEvent);
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
}


ExistingSimCenterEvents::~ExistingSimCenterEvents()
{

}


void ExistingSimCenterEvents::addEvent(void)
{
   InputWidgetExistingEvent *theExisting = new InputWidgetExistingEvent(theRandVariableIW);
   ExistingEvent *theEvent = new ExistingEvent(theRandVariableIW, theExisting);
   theEvents.append(theEvent);
   eventLayout->insertWidget(eventLayout->count()-1, theEvent);

 //  connect(this,SLOT(InputWidgetExistingEventErrorMessage(QString)), theEvent, SIGNAL(sendErrorMessage(QString)));
}


void ExistingSimCenterEvents::removeEvents(void)
{
    // find the ones selected & remove them
    int numInputWidgetExistingEvents = theEvents.size();
    for (int i = numInputWidgetExistingEvents-1; i >= 0; i--) {
      ExistingEvent *theEvent = theEvents.at(i);
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
ExistingSimCenterEvents::clear(void)
{
  // loop over random variables, removing from layout & deleting
  for (int i = 0; i <theEvents.size(); ++i) {
    ExistingEvent *theEvent = theEvents.at(i);
    eventLayout->removeWidget(theEvent);
    delete theEvent;
  }
  theEvents.clear();
}


bool
ExistingSimCenterEvents::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "ExistingSimCenterEvents";

    bool result = true;
    QJsonArray theArray;
    for (int i = 0; i <theEvents.size(); ++i) {
        QJsonObject rv;
        if (theEvents.at(i)->outputToJSON(rv)) {
            theArray.append(rv);

        } else {
            qDebug() << "OUTPUT FAILED" << theEvents.at(i)->file->text();
            result = false;
        }
    }
    jsonObject["Events"]=theArray;
    return result;
}

bool
ExistingSimCenterEvents::inputFromJSON(QJsonObject &rvObject)
{
  bool result = true;

  // clean out current list
  this->clear();

  //
  // go get InputWidgetExistingEvents array from the JSON object
  // for each object in array:
  //    1)get it'is type,
  //    2)instantiate one
  //    4) get it to input itself
  //    5) finally add it to layout
  //

  // get array

  if (rvObject.contains("Events"))
      if (rvObject["Events"].isArray()) {

          QJsonArray rvArray = rvObject["Events"].toArray();

          // foreach object in array
          foreach (const QJsonValue &rvValue, rvArray) {

              QJsonObject rvObject = rvValue.toObject();
              ExistingEvent *theEvent = new ExistingEvent(theRandVariableIW);

              if (theEvent->inputFromJSON(rvObject)) { // this method is where type is set
                  theEvents.append(theEvent);
                  eventLayout->insertWidget(eventLayout->count()-1, theEvent);
              } else {
                  result = false;
              }
          }
      }


  return result;
}


bool
ExistingSimCenterEvents::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["EventClassification"]="Earthquake";
    jsonObject["Application"] = "ExistingSimCenterEvents";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    return true;
}

bool
ExistingSimCenterEvents::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}

bool 
ExistingSimCenterEvents::copyFiles(QString &dirName) {
    for (int i = 0; i <theEvents.size(); ++i) {
        QString fileName = theEvents.at(i)->file->text();
        if (this->copyFile(fileName, dirName) ==  false) {
            emit errorMessage(QString("ERROR: copyFiles: failed to copy") + theEvents.at(i)->theName->text());
            return false;
        }
    }
    return true;
}

void
ExistingSimCenterEvents::errorMessage(QString message){
    emit sendErrorMessage(message);
}

