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
#include <RunLocalWidget.h>
#include <QProcess>
#include <QCoreApplication>


#include "GeneralInformationWidget.h"
#include <InputWidgetBIM_Selection.h>
#include <RandomVariableInputWidget.h>
#include <InputWidgetSampling.h>
#include <InputWidgetOpenSeesAnalysis.h>
#include <QDir>
#include <QFile>

InputWidgetEE_UQ::InputWidgetEE_UQ(QWidget *parent) : QWidget(parent)
{

    //
    // create the various widgets
    //

    theRVs = new RandomVariableInputWidget();
    theGI = new GeneralInformationWidget();
    theSIM = new InputWidgetBIM_Selection(theRVs);
    theEvent = new InputWidgetEarthquakeEvent(theRVs);
    theAnalysis = new InputWidgetOpenSeesAnalysis(theRVs);
    theUQ = new InputWidgetSampling();

    theResults = new SimCenterWidget();
    theRunLocalWidget = new RunLocalWidget(theUQ);

    //
    // connect signals and slots
    //

    connect(theRunLocalWidget, SIGNAL(runButtonPressed(QString, QString)), this, SLOT(runLocal(QString, QString)));

    //
    //  NOTE: for displaying the widgets we will use a QTree View to label the widgets for selection
    //  and we will use a QStacked widget for displaying the widget. Which of widgets displayed in StackedView depends on
    //  item selected in tree view.
    //

    //
    // create layout to hold tree view and stackedwidget
    //

    horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);

    //
    // create a TreeView widget & provide items for each widget to be displayed & add to layout
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
    //QStandardItem *uqItem = new QStandardItem("UQM");
    QStandardItem *resultsItem = new QStandardItem("RES");

    //building up the hierarchy of the model
    rootNode->appendRow(giItem);
    rootNode->appendRow(bimItem);
    rootNode->appendRow(evtItem);
    rootNode->appendRow(anaItem);
    rootNode->appendRow(rvItem);
    //rootNode->appendRow(uqItem);
    rootNode->appendRow(resultsItem);

    infoItemIdx = rootNode->index();

    //register the model
    treeView->setModel(standardModel);
    treeView->expandAll();
    treeView->setHeaderHidden(true);
    treeView->setMaximumWidth(100);

    //
    // set up so that a slection change triggers the selectionChanged slot
    //

    QItemSelectionModel *selectionModel= treeView->selectionModel();
    connect(selectionModel,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

    // add the TreeView widget to the layout
    horizontalLayout->addWidget(treeView);

    //
    // create the staked widget, and add to it the widgets to be displayed, and add the stacked widget itself to layout
    //

    theStackedWidget = new QStackedWidget();
    theStackedWidget->addWidget(theGI);
    theStackedWidget->addWidget(theSIM);
    theStackedWidget->addWidget(theEvent);
    theStackedWidget->addWidget(theAnalysis);
    theStackedWidget->addWidget(theRVs);
    // theStackedWidget->addWidget(theUQ);
    theStackedWidget->addWidget(theResults);

    // add stacked widget to layout
    horizontalLayout->addWidget(theStackedWidget);

    // set current selection to GI
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


void
InputWidgetEE_UQ::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/) {

    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    if (selectedText == "GEN")
        theStackedWidget->setCurrentIndex(0);
    else if (selectedText == "SIM")
        theStackedWidget->setCurrentIndex(1);
    else if (selectedText == "EVT")
        theStackedWidget->setCurrentIndex(2);
    else if (selectedText == "ANA")
        theStackedWidget->setCurrentIndex(3);
    else if (selectedText == "RVs")
        theStackedWidget->setCurrentIndex(4);
    //else if (selectedText == "UQM")
    //   theStackedWidget->setCurrentIndex(5);
    else if (selectedText == "RES")
        theStackedWidget->setCurrentIndex(5);
}


bool
InputWidgetEE_UQ::outputToJSON(QJsonObject &jsonObjectTop) {
    //
    // get each of the main widgets to output themselves
    //

    QJsonObject apps;

    QJsonObject jsonObjGenInfo;
    theGI->outputToJSON(jsonObjGenInfo);
    jsonObjectTop["GeneralInformation"] = jsonObjGenInfo;

    QJsonObject jsonObjStructural;
    theSIM->outputToJSON(jsonObjStructural);
    jsonObjectTop["StructuralInformation"] = jsonObjStructural;
    QJsonObject appsSIM;
    theSIM->outputAppDataToJSON(appsSIM);
    apps["Modeling"]=appsSIM;

    // FMK - note to self, random varaibales need to be changed
    //QJsonObject jsonObjectRVs;
    //theRVs->outputToJSON(jsonObjectRVs);
    //jsonObjectTop["RandomVariables"] = jsonObjectRVs;
    theRVs->outputToJSON(jsonObjectTop);

    QJsonObject appsEDP;
    appsEDP["Application"] = "StandardEarthquakeEDP";
    QJsonObject dataObj;
    appsEDP["ApplicationData"] = dataObj;
    apps["EDP"] = appsEDP;

    QJsonObject jsonObjectUQ;
    theUQ->outputToJSON(jsonObjectUQ);
    jsonObjectTop["UQ_Method"] = jsonObjectUQ;

    QJsonObject appsUQ;
    theUQ->outputAppDataToJSON(appsUQ);
    apps["UQ"]=appsUQ;

    QJsonObject jsonObjectAna;
    theAnalysis->outputToJSON(jsonObjectAna);
    jsonObjectTop["Simulation"] = jsonObjectAna;

    QJsonObject appsAna;
    theAnalysis->outputAppDataToJSON(appsAna);
    apps["Simulation"]=appsAna;


   // NOTE: Events treated differently, due to array nature of objects
    theEvent->outputToJSON(jsonObjectTop);
    theEvent->outputAppDataToJSON(apps);


    jsonObjectTop["Applications"]=apps;

    theRunLocalWidget->outputToJSON(jsonObjectTop);

    return true;
}

void
InputWidgetEE_UQ::clear(void)
{
    theGI->clear();
    theSIM->clear();
}

bool
InputWidgetEE_UQ::inputFromJSON(QJsonObject &jsonObject)
{

    //
    // get each of the main widgets to input themselves
    //

    if (jsonObject.contains("GeneralInformation")) {
        QJsonObject jsonObjGeneralInformation = jsonObject["GeneralInformation"].toObject();
        theGI->inputFromJSON(jsonObjGeneralInformation);
    } else
        return false;

    if (jsonObject.contains("StructuralInformation")) {
        QJsonObject jsonObjStructuralInformation = jsonObject["StructuralInformation"].toObject();
        theSIM->inputFromJSON(jsonObjStructuralInformation);
    } else
        return false;

    /*
    ** Note to me - RVs and Events treated differently as both use arrays .. rethink API!
    */

    theEvent->inputFromJSON(jsonObject);
    theRVs->inputFromJSON(jsonObject);

    /*
    if (jsonObject.contains("Events")) {
        QJsonObject jsonObjEventInformation = jsonObject["Event"].toObject();
        theEvent->inputFromJSON(jsonObjEventInformation);
    } else
        return false;

    if (jsonObject.contains("RandomVariables")) {
        QJsonObject jsonObjRVsInformation = jsonObject["RandomVariables"].toObject();
        theRVS->inputFromJSON(jsonObRVSInformation);
    } else
        return false;
    */

    if (jsonObject.contains("UQ_Method")) {
        QJsonObject jsonObjUQInformation = jsonObject["UQ"].toObject();
        theEvent->inputFromJSON(jsonObjUQInformation);
    } else
        return false;

    if (jsonObject.contains("Applications")) {

        QJsonObject theApplicationObject = jsonObject["Applications"].toObject();

        if (theApplicationObject.contains("Modeling")) {
            QJsonObject theObject = theApplicationObject["Modeling"].toObject();
            theSIM->inputAppDataFromJSON(theObject);
        } else
            return false;

        // note: Events is different because the object is an Array
        if (theApplicationObject.contains("Events")) {
            QJsonObject theObject = theApplicationObject["Events"].toObject();
            theEvent->inputAppDataFromJSON(theApplicationObject);
        } else
            return false;


        if (theApplicationObject.contains("UQ")) {
            QJsonObject theObject = theApplicationObject["UQ"].toObject();
            theUQ->inputAppDataFromJSON(theObject);
        } else
            return false;

        if (theApplicationObject.contains("Simulation")) {
            QJsonObject theObject = theApplicationObject["Simulation"].toObject();
            theAnalysis->inputAppDataFromJSON(theObject);
        } else
            return false;


    } else
        return false;

    return theRunLocalWidget->inputFromJSON(jsonObject);

    //return true;
}


void
InputWidgetEE_UQ::onRunButtonClicked() {
    theRunLocalWidget->show();
}

void
InputWidgetEE_UQ::onRemoteRunButtonClicked(){

}
void
InputWidgetEE_UQ::onRemoteGetButtonClicked(){

};

void
InputWidgetEE_UQ::onExitButtonClicked(){

}

void
InputWidgetEE_UQ::runLocal(QString workingDir, QString appDir) {

    qDebug() << "EE_UQ - RUN LOCAL workigDir" << workingDir << " appDir: " << appDir;
    theRunLocalWidget->hide();

   // errorMessage("");


    //
    // create temporary directory in working dir
    // and copy all files needed to this directory by invoking copyFiles() on app widgets
    //

    QString tmpDirectory = workingDir + QDir::separator() + QString("tmp.SimCenter"); // + QDir::separator() + QString("templatedir");
    QDir destinationDirectory(tmpDirectory);

    if(destinationDirectory.exists()) {
      destinationDirectory.removeRecursively();
    } else
        destinationDirectory.mkpath(tmpDirectory);

    QString templateDirectory  = tmpDirectory + QDir::separator() + QString("templatedir");
    destinationDirectory.mkpath(templateDirectory);

    // copyPath(path, tmpDirectory, false);
    theSIM->copyFiles(templateDirectory);
    theEvent->copyFiles(templateDirectory);
    theAnalysis->copyFiles(templateDirectory);
    theUQ->copyFiles(templateDirectory);

    //
    // in new templatedir dir save the UI data into dakota.json file (same result as using saveAs)
    // NOTE: we append object workingDir to this which points to template dir
    //

    QString inputFile = templateDirectory + QDir::separator() + tr("dakota.json");

    QFile file(inputFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        //errorMessage();
        return;
    }
    QJsonObject json;
    this->outputToJSON(json);

    json["runDir"]=tmpDirectory;
    json["WorkflowType"]="Building Simulation";


    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    //
    // now use the applications Workflow Application EE-UQ.py  to run dakota and produce output files:
    //    dakota.in dakota.out dakotaTab.out dakota.err
    //


    QString pySCRIPT = appDir +  QDir::separator() + "applications" + QDir::separator() + "Workflow" + QDir::separator() +
            QString("EE-UQ.py");

    QString registryFile = appDir +  QDir::separator() + "applications" + QDir::separator() + "Workflow" + QDir::separator() +
            QString("WorkflowApplications.json");
    qDebug() << pySCRIPT;


    QStringList files;
    files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";

   /************************************************************************
    for (int i = 0; i < files.size(); i++) {
        QString copy = files.at(i);
        QFile file(destinationDir + copy);
        file.remove();
    }
    ***********************************************************************/

    //
    // now invoke dakota, done via a python script in tool app dircetory
    //

    QProcess *proc = new QProcess();

#ifdef Q_OS_WIN
    QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") + tmpDirectory  + QString(" runningLocal");
    qDebug() << command;
    proc->execute("cmd", QStringList() << "/C" << command);
    //   proc->start("cmd", QStringList(), QIODevice::ReadWrite);

#else
   QString command = QString("source $HOME/.bash_profile; python ") + pySCRIPT + QString(" run ") + inputFile + QString(" ") +
     registryFile;

    proc->execute("bash", QStringList() << "-c" <<  command);

    qInfo() << command;

#endif
    proc->waitForStarted();

    //
    // process the results
    //
/*
    QString filenameOUT = destinationDir + tr("dakota.out");
    QString filenameTAB = destinationDir + tr("dakotaTab.out");
*/
   // this->processResults(filenameOUT, filenameTAB);

}
