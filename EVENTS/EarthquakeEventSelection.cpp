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

#include "EarthquakeEventSelection.h"

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
#include <sectiontitle.h>
//#include <InputWidgetEDP.h>

#include <InputWidgetExistingEvent.h>
#include <ExistingSimCenterEvents.h>
#include <ExistingPEER_Records.h>
#include <UserDefinedApplication.h>
#include "StochasticMotionInput.h"
#include "RockOutcrop.h"
#include "peerNGA/PEER_NGA_Records.h"


EarthquakeEventSelection::EarthquakeEventSelection(RandomVariablesContainer *theRandomVariableIW, GeneralInformationWidget* generalInfoWidget, QWidget *parent)
    : SimCenterAppWidget(parent), theCurrentEvent(0), theRandomVariablesContainer(theRandomVariableIW)
{
    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    //    QLabel *label = new QLabel();
    SectionTitle *label=new SectionTitle();
    label->setMinimumWidth(250);
    label->setText(QString("Load Generator"));

    eventSelection = new QComboBox();
    eventSelection->setObjectName("LoadingTypeCombox");

    eventSelection->addItem(tr("Stochastic Ground Motion"));
    eventSelection->addItem(tr("PEER NGA Records"));
    eventSelection->addItem(tr("Site Response"));
    eventSelection->addItem(tr("Multiple PEER"));
    eventSelection->addItem(tr("Multiple SimCenter"));
    //    eventSelection->addItem(tr("Hazard Based Event"));
    // eventSelection->addItem(tr("User Application"));
    eventSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    eventSelection->setItemData(1, "A Seismic event using Seismic Hazard Analysis and Record Selection/Scaling", Qt::ToolTipRole);

    theSelectionLayout->addWidget(label);
    QSpacerItem *spacer = new QSpacerItem(50,10);
    theSelectionLayout->addItem(spacer);
    theSelectionLayout->addWidget(eventSelection);
    theSelectionLayout->addStretch();
    layout->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //
    theStackedWidget = new QStackedWidget();

    //
    // create the individual load widgets & add to stacked widget
    //

    //theExistingEventsWidget = new InputWidgetExistingEvent(theRandomVariablesContainer);
    //theStackedWidget->addWidget(theExistingEventsWidget);

    // Adding stochastic ground motion model widget
    theStochasticMotionWidget = new StochasticMotionInput(theRandomVariablesContainer);
    theStackedWidget->addWidget(theStochasticMotionWidget);

    //Adding PEER NGA Records ground motion widget
    peerNgaRecords = new PEER_NGA_Records(generalInfoWidget, this);
    theStackedWidget->addWidget(peerNgaRecords);

    //Adding SHA based ground motion widget
    /*
    theSHA_MotionWidget = new SHAMotionWidget(theRandomVariablesContainer);
    theStackedWidget->addWidget(theSHA_MotionWidget);
    */

    // Adding SRT widget
    theRockOutcrop = new RockOutcrop(theRandomVariablesContainer);
    theStackedWidget->addWidget(theRockOutcrop);

    theExistingPeerEvents = new ExistingPEER_Records(theRandomVariablesContainer);
    theStackedWidget->addWidget(theExistingPeerEvents);

    // Adding stochastic ground motion model widget
    theExistingEvents = new ExistingSimCenterEvents(theRandomVariablesContainer);
    theStackedWidget->addWidget(theExistingEvents);

    //    theUserDefinedApplication = new UserDefinedApplication(theRandomVariablesContainer);
    //theStackedWidget->addWidget(theUserDefinedApplication);


    layout->addWidget(theStackedWidget);
    layout->setMargin(0);
    this->setLayout(layout);
    theCurrentEvent=theStochasticMotionWidget;

    //
    // connect signal and slots
    //

    connect(eventSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(eventSelectionChanged(QString)));

    connect(theStochasticMotionWidget, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    connect(theStochasticMotionWidget, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    connect(theStochasticMotionWidget, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});

    connect(theRockOutcrop, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    connect(theRockOutcrop, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    connect(theRockOutcrop, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});

    //connect(theSHA_MotionWidget, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    //connect(theSHA_MotionWidget, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    //connect(theSHA_MotionWidget, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});

    connect(theExistingEvents, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    connect(theExistingEvents, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    connect(theExistingEvents, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});

    connect(theExistingPeerEvents, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    connect(theExistingPeerEvents, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    connect(theExistingPeerEvents, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});

    connect(peerNgaRecords, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    connect(peerNgaRecords, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    connect(peerNgaRecords, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});
}

EarthquakeEventSelection::~EarthquakeEventSelection()
{

}


bool
EarthquakeEventSelection::outputToJSON(QJsonObject &jsonObject)
{
    QJsonArray eventArray;
    QJsonObject singleEventData;
    theCurrentEvent->outputToJSON(singleEventData);
    eventArray.append(singleEventData);
    jsonObject["Events"]=eventArray;

    return true;
}


bool
EarthquakeEventSelection::inputFromJSON(QJsonObject &jsonObject) {

    QString type;
    QJsonObject theEvent;

    if (jsonObject.contains("Events")) {
        QJsonArray theEvents = jsonObject["Events"].toArray();
        QJsonValue theValue = theEvents.at(0);
        if (theValue.isNull()) {
            qDebug() << "EarthquakeEventSelection::no Event in Events";
            return false;
        }
        theEvent = theValue.toObject();

    } else {
        qDebug() << "EarthquakeEventSelection::no Events";
        return false;
    }

    if (theCurrentEvent != 0) {
        return theCurrentEvent->inputFromJSON(theEvent);
    }

    return false;
}

void EarthquakeEventSelection::eventSelectionChanged(const QString &arg1)
{
    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == "Multiple SimCenter") {
        theStackedWidget->setCurrentIndex(4);
        theCurrentEvent = theExistingEvents;
    }

    else if(arg1 == "Multiple PEER") {
        theStackedWidget->setCurrentIndex(3);
        theCurrentEvent = theExistingPeerEvents;
    }
    
    /*
    else if(arg1 == "Hazard Based Event") {
        theStackedWidget->setCurrentIndex(3);
        theCurrentEvent = theSHA_MotionWidget;
    }
    */

    else if (arg1 == "Site Response") {
      theStackedWidget->setCurrentIndex(2);
      theCurrentEvent = theRockOutcrop;
    }

    else if (arg1 == "Stochastic Ground Motion") {
      theStackedWidget->setCurrentIndex(0);
      theCurrentEvent = theStochasticMotionWidget;
    }

    /*
    else if(arg1 == "User Application") {
        theStackedWidget->setCurrentIndex(6);
        theCurrentEvent = theUserDefinedApplication;
    }
    */

    else if(arg1 == "PEER NGA Records") {
        theStackedWidget->setCurrentIndex(1);
        theCurrentEvent = peerNgaRecords;
    }
    else {
        qDebug() << "ERROR .. EarthquakeEventSelection selection .. type unknown: " << arg1;
    }
}

bool
EarthquakeEventSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonArray eventArray;
    QJsonObject singleEventData;
    theCurrentEvent->outputAppDataToJSON(singleEventData);
    eventArray.append(singleEventData);
    jsonObject["Events"]=eventArray;
    return true;
}


bool
EarthquakeEventSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
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
    if ((type == QString("Existing Events")) ||
	(type == QString("Existing SimCenter Events")) ||
	(type == QString("ExistingSimCenterEvents"))) {
        index = 4;
    } else if ((type == QString("Existing PEER Records")) ||
               (type == QString("ExistingPEER_Events"))  ||
               (type == QString("ExistingPEER_Records"))) {
        if(!subtype.isEmpty() && subtype == "PEER NGA Records")
            index = 1;
        else
            index = 3;
  //  } else if (type == QString("Hazard Based Event")) {
  //      index = 3;
    } else if (type == QString("Site Response") ||
               type == QString("SiteResponse")) {
        index = 2;
    } else if (type == QString("Stochastic Ground Motion Model") ||
	       type == QString("Stochastic Ground Motion") ||
	       type == QString("StochasticGroundMotion") ||
               type == QString("StochasticMotion")) {
        index = 0;
   // } else if ((type == QString("User Application")) ||
   //            (type == QString("UserDefinedApplication"))) {
   //     index = 6;
    } else {
        return false;
    }

    eventSelection->setCurrentIndex(index);

    // invoke inputAppDataFromJSON on new type
    if (theCurrentEvent != 0 && !theEvent.isEmpty()) {
        return theCurrentEvent->inputAppDataFromJSON(theEvent);
    }
}

bool
EarthquakeEventSelection::copyFiles(QString &destDir) {

    if (theCurrentEvent != 0) {
        return  theCurrentEvent->copyFiles(destDir);
    }

    return false;
}
