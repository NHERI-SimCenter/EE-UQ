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

#include "InputWidgetEE_UQ.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStackedWidget>
#include <InputWidgetEarthquakeEvent.h>


#include "GeneralInformationWidget.h"
#include <InputWidgetSheetSIM.h>
#include <RandomVariableInputWidget.h>
#include <InputWidgetSampling.h>

InputWidgetEE_UQ::InputWidgetEE_UQ(QWidget *parent) : QWidget(parent)
{
  horizontalLayout = new QHBoxLayout();
  this->setLayout(horizontalLayout);

  //
  // create a tree widget, assign it a mode and add to layout
  //
  treeView = new QTreeView();
  standardModel = new QStandardItemModel ;
  QStandardItem *rootNode = standardModel->invisibleRootItem();

  //defining bunch of items for inclusion in model
  QStandardItem *giItem    = new QStandardItem("GEN");
  QStandardItem *rvItem   = new QStandardItem("RVs");
  QStandardItem *bimItem = new QStandardItem("SIM");
  QStandardItem *evtItem = new QStandardItem("EVT");
  QStandardItem *anaItem = new QStandardItem("ANA");
  QStandardItem *uqItem = new QStandardItem("UQM");
  QStandardItem *resultsItem = new QStandardItem("RES");

  //building up the hierarchy of the model
  rootNode->appendRow(giItem);
  rootNode->appendRow(rvItem);
  rootNode->appendRow(bimItem);
  rootNode->appendRow(evtItem);
  rootNode->appendRow(anaItem);
  rootNode->appendRow(uqItem);
  rootNode->appendRow(resultsItem);

  infoItemIdx = rootNode->index();

  //register the model
  treeView->setModel(standardModel);
  treeView->expandAll();
  treeView->setHeaderHidden(true);
  treeView->setMaximumWidth(100);

  // set up so that a slection change triggers yje selectionChanged slot
  QItemSelectionModel *selectionModel= treeView->selectionModel();
  connect(selectionModel,
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this,
          SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

  // add the TreeView widget to the layout
  horizontalLayout->addWidget(treeView);


  //
  // create the input widgets for the different types
  //
  theStackedWidget = new QStackedWidget();


  theGI = new GeneralInformationWidget();
  theRVs = new RandomVariableInputWidget();
  theSIM = new InputWidgetSheetSIM();
  theEvent = new InputWidgetEarthquakeEvent();
  theUQ = new InputWidgetSampling();
  theAnalysisOptions = new SimCenterWidget();
  theResults = new SimCenterWidget();

  theStackedWidget->addWidget(theGI);
  theStackedWidget->addWidget(theRVs);
  theStackedWidget->addWidget(theSIM);
  theStackedWidget->addWidget(theEvent);
  theStackedWidget->addWidget(theAnalysisOptions);
  theStackedWidget->addWidget(theUQ);
  theStackedWidget->addWidget(theResults);

  horizontalLayout->addWidget(theStackedWidget);

  treeView->setCurrentIndex( infoItemIdx );

}

InputWidgetEE_UQ::~InputWidgetEE_UQ()
{

}

void InputWidgetEE_UQ::setMainWindow(MainWindow* main)
{
    window = main;
    treeView->setCurrentIndex( infoItemIdx );
}


void InputWidgetEE_UQ::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{

    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    if (selectedText == "GEN")
        theStackedWidget->setCurrentIndex(0);
    else if (selectedText == "RVs")
        theStackedWidget->setCurrentIndex(1);
    else if (selectedText == "SIM")
        theStackedWidget->setCurrentIndex(2);
    else if (selectedText == "EVT")
         theStackedWidget->setCurrentIndex(3);
    else if (selectedText == "ANA")
         theStackedWidget->setCurrentIndex(4);
    else if (selectedText == "UQM")
        theStackedWidget->setCurrentIndex(5);
    else if (selectedText == "RES")
        theStackedWidget->setCurrentIndex(6);
  }



void
InputWidgetEE_UQ::outputToJSON(QJsonObject &jsonObjectTop)
{
    QJsonObject jsonObject;

    // add GeneralInformation
    QJsonObject jsonObjGenInfo;
    theGI->outputToJSON(jsonObjGenInfo);
    jsonObjectTop["GeneralInformation"] = jsonObjGenInfo;

    QJsonObject jsonObjStructural;
    theSIM->outputToJSON(jsonObjStructural);
    jsonObjectTop["StructuralInformation"] = jsonObjStructural;


/*
    // add layout
    QJsonObject jsonObjLayout;
    theClineInput->outputToJSON(jsonObjLayout);
    theFloorInput->outputToJSON(jsonObjLayout);
    jsonObject["layout"]=jsonObjLayout;

    // add geometry
    QJsonObject jsonObjGeometry;
    theBeamInput->outputToJSON(jsonObjGeometry);
    theColumnInput->outputToJSON(jsonObjGeometry);
    theBraceInput->outputToJSON(jsonObjGeometry);

    jsonObject["geometry"]=jsonObjGeometry;

    // add properties
    QJsonObject jsonObjProperties;
    theSlabsectionInput->outputToJSON(jsonObjProperties);
    thePointInput->outputToJSON(jsonObjProperties);

    QJsonArray theFramesectionsArray;
    jsonObjProperties["framesections"]=theFramesectionsArray;

    for (int i=0; i<theFramesectionTypes.size(); i++) {
        theFramesectionInputs[theFramesectionTypes[i]]->outputToJSON(theFramesectionsArray);
    }
    jsonObjProperties["framesections"]=theFramesectionsArray;

    QJsonArray theWallsectionsArray;
    jsonObjProperties["wallsections"]=theWallsectionsArray;

    for (int i=0; i<theWallsectionTypes.size(); i++) {
        theWallsectionInputs[theWallsectionTypes[i]]->outputToJSON(theWallsectionsArray);
    }
    jsonObjProperties["wallsections"]=theWallsectionsArray;

    QJsonArray theConnectionsArray;
    jsonObjProperties["connections"]=theConnectionsArray;

    for (int i=0; i<theConnectionTypes.size(); i++) {
        theConnectionInputs[theConnectionTypes[i]]->outputToJSON(theConnectionsArray);
    }
    jsonObjProperties["connections"]=theConnectionsArray;

    //
    // create a json array and get all material inputs to enter their data
    //
    QJsonArray theMaterialsArray;
    jsonObjProperties["materials"]=theMaterialsArray;

    theSteelInput->outputToJSON(theMaterialsArray);
    theConcreteInput->outputToJSON(theMaterialsArray);
    jsonObjProperties["materials"]=theMaterialsArray;


    jsonObject["properties"]=jsonObjProperties;

    jsonObjectTop["StructuralInformation"] = jsonObject;
    */

}

void
InputWidgetEE_UQ::clear(void)
{
    /*
    theGeneralInformationInput->clear();
    theClineInput->clear();
    theFloorInput->clear();
    theColumnInput->clear();
    theBeamInput->clear();
    theBraceInput->clear();
    theSteelInput->clear();
    theConcreteInput->clear();

    for (int i=0; i<theFramesectionTypes.size(); i++) {
        theFramesectionInputs[theFramesectionTypes[i]]->clear();
    }

    theSlabsectionInput->clear();

    for (int i=0; i<theWallsectionTypes.size(); i++) {
        theWallsectionInputs[theWallsectionTypes[i]]->clear();
    }

    for (int i=0; i<theConnectionTypes.size(); i++) {
        theConnectionInputs[theConnectionTypes[i]]->clear();
    }

    thePointInput->clear();

    if (jsonObjOrig) {
        delete jsonObjOrig;
    }
    */
}

void
InputWidgetEE_UQ::inputFromJSON(QJsonObject &jsonObject)
{

   jsonObjOrig = new QJsonObject(jsonObject);

   QJsonObject jsonObjGeneralInformation = jsonObject["GeneralInformation"].toObject();
   theGI->inputFromJSON(jsonObjGeneralInformation);


   QJsonObject jsonObjStructuralInformation = jsonObject["StructuralInformation"].toObject();
   theSIM->inputFromJSON(jsonObjStructuralInformation);

   /*
   QJsonObject jsonObjLayout = jsonObjStructuralInformation["layout"].toObject();
   theClineInput->inputFromJSON(jsonObjLayout);
   theFloorInput->inputFromJSON(jsonObjLayout);

   //
   // parse the properties
   //

   QJsonObject jsonObjProperties = jsonObjStructuralInformation["properties"].toObject();
   theSlabsectionInput->inputFromJSON(jsonObjProperties);
   thePointInput->inputFromJSON(jsonObjProperties);

   QJsonArray theFramesectionsArray = jsonObjProperties["framesections"].toArray();
   for (int i=0; i<theFramesectionTypes.size(); i++) {
       theFramesectionInputs[theFramesectionTypes[i]]->inputFromJSON(theFramesectionsArray);
   }

   QJsonArray theWallsectionsArray = jsonObjProperties["wallsections"].toArray();
   for (int i=0; i<theWallsectionTypes.size(); i++) {
       theWallsectionInputs[theWallsectionTypes[i]]->inputFromJSON(theWallsectionsArray);
   }

   QJsonArray theConnectionsArray = jsonObjProperties["connections"].toArray();
   for (int i=0; i<theConnectionTypes.size(); i++) {
       theConnectionInputs[theConnectionTypes[i]]->inputFromJSON(theConnectionsArray);
   }

   // first the materials
   // get the array and for every object in array determine it's type and get
   // the approprate inputwidget to parse the data
   //

   QJsonArray theMaterialArray = jsonObjProperties["materials"].toArray();
   foreach (const QJsonValue &theValue, theMaterialArray) {

       QJsonObject theObject = theValue.toObject();
       QString theType = theObject["type"].toString();

       if (theType == QString(tr("steel"))) {
            theSteelInput->inputFromJSON(theObject);
       } else if (theType == QString(tr("concrete"))) {
           theConcreteInput->inputFromJSON(theObject);
      }
   }


   //
   // parse the geometry
   //

   QJsonObject jsonObjGeometry = jsonObjStructuralInformation["geometry"].toObject();
   theColumnInput->inputFromJSON(jsonObjGeometry);
   theBeamInput->inputFromJSON(jsonObjGeometry);
   theBraceInput->inputFromJSON(jsonObjGeometry);
*/

}


