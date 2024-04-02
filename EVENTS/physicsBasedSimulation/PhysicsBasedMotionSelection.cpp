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

#include "PhysicsBasedMotionSelection.h"
#include "M9SingleSite.h"
#include <GoogleAnalytics.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStackedWidget>
#include <QComboBox>
#include <QSpacerItem>

#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>

#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <SectionTitle.h>
//#include <InputWidgetEDP.h>

#include <QScrollArea>
#include <RandomVariablesContainer.h>
#include <GeneralInformationWidget.h>

PhysicsBasedMotionSelection::PhysicsBasedMotionSelection(GeneralInformationWidget* generalInfoWidget,
							 QWidget *parent)
    : SimCenterAppWidget(parent), theCurrentEvent(0)
{
    QGridLayout *layout = new QGridLayout();

    
    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    SectionTitle *label=new SectionTitle();
    label->setMinimumWidth(250);
    label->setText(QString("Physics Based Motion Type"));

    eventSelection = new QComboBox();
    eventSelection->setObjectName("LoadingTypeCombox");

    eventSelection->addItem(tr("M9"));
    eventSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    eventSelection->setMinimumWidth(250);
    eventSelection->setMaximumHeight(25);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(eventSelection);
    theSelectionLayout->addStretch();
    layout->addLayout(theSelectionLayout,1,1);

    //
    // create the stacked widget
    //

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);      
    
    theStackedWidget = new QStackedWidget();
    layout->addWidget(theStackedWidget,2,1,6,1);

    // Adding M9
    theM9 = new M9SingleSite();
    theStackedWidget->addWidget(theM9);

    QGroupBox *allWidgets = new QGroupBox();
    allWidgets->setLayout(layout);

    sa->setWidget(allWidgets);

    QVBoxLayout *global_layout = new QVBoxLayout();
    global_layout->addWidget(sa);

    this->setLayout(global_layout);
    theCurrentEvent=theM9;

    //
    // connect signal and slots
    //

    connect(eventSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(eventSelectionChanged(QString)));

    global_layout->setSpacing(0);
    layout->setSpacing(0);
    global_layout->setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);

}

PhysicsBasedMotionSelection::~PhysicsBasedMotionSelection()
{

}


bool
PhysicsBasedMotionSelection::outputToJSON(QJsonObject &jsonObject)
{
  /*
    QJsonArray eventArray;
    QJsonObject singleEventData;
    
    bool result = theCurrentEvent->outputToJSON(singleEventData);
    eventArray.append(singleEventData);
    jsonObject["Events"]=eventArray;
  */
  bool result = theCurrentEvent->outputToJSON(jsonObject);  
    return result;
}


bool
PhysicsBasedMotionSelection::inputFromJSON(QJsonObject &jsonObject) {

    QString type;
    QJsonObject theEvent;

    if (jsonObject.contains("Events")) {
        QJsonArray theEvents = jsonObject["Events"].toArray();
        QJsonValue theValue = theEvents.at(0);
        if (theValue.isNull()) {
            qDebug() << "PhysicsBasedMotionSelection::no Event in Events";
            return false;
        }
        theEvent = theValue.toObject();

    } else {
        qDebug() << "PhysicsBasedMotionSelection::no Events";
        return false;
    }

    if (theCurrentEvent != 0) {
        return theCurrentEvent->inputFromJSON(theEvent);
    }

    return false;
}

void PhysicsBasedMotionSelection::eventSelectionChanged(const QString &arg1)
{
    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == "M9") {
      
        theStackedWidget->setCurrentIndex(0);
        theCurrentEvent = theM9;
	currentEventType="M9";
	
    } else {
        qDebug() << "ERROR .. PhysicsBasedMotionSelection selection .. type unknown: " << arg1;
    }
}

bool
PhysicsBasedMotionSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["Application"] = "PhysicsBasedMotion";
    jsonObject["subtype"] = eventSelection->currentText();
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    return true;  
}


bool
PhysicsBasedMotionSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    QJsonObject theEvent;
    QString type;
    QString subtype;

    // from Events get the single event

    if (jsonObject.contains("Events")) {
        QJsonArray theEvents = jsonObject["Events"].toArray();
        QJsonValue theValue = theEvents.at(0);
        if (theValue.isNull()) {
          return false;
        }
        theEvent = theValue.toObject();
        if (theEvent.contains("Application")) {
            QJsonValue theName = theEvent["Application"];
            type = theName.toString();
            if(theEvent.contains("subtype"))
                subtype = theEvent["subtype"].toString();
        } else
            return false;
    } else
        return false;


    int index = 0;
    if (type == QString("M9")) {
      currentEventType="M9";
      index = 0;
    } else {
      return false;
    }
    
    eventSelection->setCurrentIndex(index);
}

bool
PhysicsBasedMotionSelection::copyFiles(QString &destDir) {

    if (theCurrentEvent != 0) {
          QString textForAnalytics = QString("Event_") + currentEventType;
	  GoogleAnalytics::ReportAppUsage(textForAnalytics);    
	  return theCurrentEvent->copyFiles(destDir);
    }

    return false;
}

void
PhysicsBasedMotionSelection::replyEventType(void) {
    if (eventSelection->currentIndex() != 2 && eventSelection->currentIndex() != 5) {
        emit typeEVT("EQ");
    } else {
        // the Site Response and User-Defined Database are excluded
        emit typeEVT("None");
    }
}

bool
PhysicsBasedMotionSelection::outputCitation(QJsonObject &jsonObject) {
  theCurrentEvent->outputCitation(jsonObject);
}
