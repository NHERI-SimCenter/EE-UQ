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

#include "WorkflowAppEE_UQ.h"
#include <MainWindowWorkflowApp.h>

#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <EarthquakeEventSelection.h>
#include <EDP_EarthquakeSelection.h>
#include <RunLocalWidget.h>
#include <QProcess>
#include <QCoreApplication>
#include <RemoteService.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QUuid>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QUuid>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QMenuBar>

#include <SimCenterComponentSelection.h>
#include "GeneralInformationWidget.h"
#include <SIM_Selection.h>
#include <RandomVariablesContainer.h>
#include <FEA_Selection.h>
#include <UQ_EngineSelection.h>
#include <UQ_Results.h>
#include <LocalApplication.h>
#include <RemoteApplication.h>
#include <RemoteJobManager.h>
#include <RunWidget.h>
#include <InputWidgetBIM.h>
#include <Stampede3Machine.h>
#include <SC_ToolDialog.h>
#include <SC_RemoteAppTool.h>
#include <RemoteOpenSeesApp.h>
#include <QList>
#include <ShakerMaker.h>
#include <peerNGA/PEER_NGA_Records.h>
#include "CustomizedItemModel.h"

#include <Utils/ProgramOutputDialog.h>
#include <Utils/RelativePathResolver.h>
#include <GoogleAnalytics.h>

// static pointer for global procedure set in constructor
static WorkflowAppEE_UQ *theApp = 0;

extern bool isSafeToRemoveRecursivily(const QString &directoryPath);

// global procedure
int getNumParallelTasks() {
    return theApp->getMaxNumParallelTasks();
}

WorkflowAppEE_UQ::WorkflowAppEE_UQ(RemoteService *theService, QWidget *parent)
    : WorkflowAppWidget(theService, parent)
{
    // set static pointer for global procedure
    theApp = this;

    //
    // create the various widgets
    //

    theRVs = RandomVariablesContainer::getInstance();
    theGI = GeneralInformationWidget::getInstance();
    theSIM = new SIM_Selection(false, true);
    theEventSelection = new EarthquakeEventSelection(theRVs, theGI);
    theAnalysisSelection = new FEA_Selection(true);
    theUQ_Selection = new UQ_EngineSelection(ForwardReliabilitySensitivitySurrogate);

    theEDP_Selection = new EDP_EarthquakeSelection(theRVs);
      
    theResults = theUQ_Selection->getResults();

    localApp = new LocalApplication("sWHALE.py");
    remoteApp = new RemoteApplication("sWHALE.py", theService);

    //    localApp = new LocalApplication("EE-UQ workflow.py");
    //   remoteApp = new RemoteApplication("EE-UQ workflow.py", theService);

    // localApp = new LocalApplication("EE-UQ.py");
    // remoteApp = new RemoteApplication("EE-UQ.py", theService);
    theJobManager = new RemoteJobManager(theService);

    SimCenterWidget *theWidgets[1];// =0;
    theRunWidget = new RunWidget(localApp, remoteApp, theWidgets, 0);

    //
    // connect signals and slots
    //

    connect(localApp,SIGNAL(setupForRun(QString &,QString &)), this, SLOT(setUpForApplicationRun(QString &,QString &)));
    connect(this,SIGNAL(setUpForApplicationRunDone(QString&, QString &)), theRunWidget, SLOT(setupForRunApplicationDone(QString&, QString &)));
    connect(localApp,SIGNAL(processResults(QString&)), this, SLOT(processResults(QString&)));

    connect(remoteApp,SIGNAL(setupForRun(QString &,QString &)), this, SLOT(setUpForApplicationRun(QString &,QString &)));

    connect(theJobManager,SIGNAL(processResults(QString&)), this, SLOT(processResults(QString&)));
    connect(theJobManager,SIGNAL(loadFile(QString&)), this, SLOT(loadFile(QString&)));

    connect(remoteApp,SIGNAL(successfullJobStart()), theRunWidget, SLOT(hide()));

    connect(localApp,SIGNAL(runComplete()), this, SLOT(runComplete()));
    connect(remoteApp,SIGNAL(successfullJobStart()), this, SLOT(runComplete()));
    connect(theService, SIGNAL(closeDialog()), this, SLOT(runComplete()));
    connect(theJobManager, SIGNAL(closeDialog()), this, SLOT(runComplete()));

    // KZ connect queryEVT and the reply
    connect(theUQ_Selection, SIGNAL(queryEVT()), theEventSelection, SLOT(replyEventType()));
    connect(theEventSelection, SIGNAL(typeEVT(QString)), theUQ_Selection, SLOT(setEventType(QString)));

    //
    // create layout to hold component selectio
    //


    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);
    this->setContentsMargins(0,5,0,5);
    horizontalLayout->setMargin(0);

    //
    // create the component selection & add the components to it
    //

    theComponentSelection = new SimCenterComponentSelection();
    horizontalLayout->addWidget(theComponentSelection);

    theComponentSelection->addComponent(QString("UQ"),  theUQ_Selection);
    theComponentSelection->addComponent(QString("GI"),  theGI);
    theComponentSelection->addComponent(QString("SIM"), theSIM);
    theComponentSelection->addComponent(QString("EVT"), theEventSelection);
    theComponentSelection->addComponent(QString("FEM"), theAnalysisSelection);
    theComponentSelection->addComponent(QString("EDP"), theEDP_Selection);
    theComponentSelection->addComponent(QString("RV"),  theRVs);
    theComponentSelection->addComponent(QString("RES"), theResults);

    theComponentSelection->displayComponent("UQ");

    // access a web page which will increment the usage count for this tool
    /*
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/eeuq/use.php")));
    */
    

    //
    // set the defults in the General Info
    //

    theGI->setDefaultProperties(1,144,360,360,37.8715,-122.2730);

    ProgramOutputDialog *theDialog=ProgramOutputDialog::getInstance();
    theDialog->appendInfoMessage("Welcome to EE-UQ");
}

void
WorkflowAppEE_UQ::setMainWindow(MainWindowWorkflowApp* window) {

  this->WorkflowAppWidget::setMainWindow(window);

  //
  // Add a Tool option to menu bar & add options to it
  //
  
  auto menuBar = theMainWindow->menuBar();
  
  QMenu *toolsMenu = new QMenu(tr("&Tools"),menuBar);
  SC_ToolDialog *theToolDialog = new SC_ToolDialog(this);

  //
  // Add Some Tools
  //

  // peer ground motion selection
  PEER_NGA_Records *peerNGA = new PEER_NGA_Records(theGI);
  theToolDialog->addTool(peerNGA, "PEER Ground Motion Records");
  QAction *showPEER = toolsMenu->addAction("&PEER Ground Motion Records");
  connect(showPEER, &QAction::triggered, this,[this, theDialog=theToolDialog, theEmp = peerNGA] {
    theDialog->showTool("PEER Ground Motion Records");
  });
  
  // shaker maker
  ShakerMaker *theShakerMaker = new ShakerMaker();
  theToolDialog->addTool(theShakerMaker, "ShakerMaker");
  QAction *showShakerMaker = toolsMenu->addAction("&ShakerMaker");
  connect(showShakerMaker, &QAction::triggered, this,[this, theDialog=theToolDialog, theEmp = theShakerMaker] {
    theDialog->showTool("ShakerMaker");
  });

  // opensees@designsafe  
  RemoteOpenSeesApp *theOpenSeesApp = new RemoteOpenSeesApp();

  QString testAppName = "simcenter-opensees-frontera";
  QString testAppVersion = "1.0.0";
  TapisMachine *theMachine = new Stampede3Machine();
  SC_RemoteAppTool *theOpenSeesTool = new SC_RemoteAppTool(testAppName,
							   testAppVersion,
							   theMachine,
							   theRemoteService,
							   theOpenSeesApp,
							   theToolDialog);  
  
  theToolDialog->addTool(theOpenSeesTool, "OpenSees@DesignSafe");
  QAction *showOpenSees = toolsMenu->addAction("&OpenSees@DesignSafe");
  connect(showOpenSees, &QAction::triggered, this,[this, theDialog=theToolDialog, theEmp = theOpenSeesApp] {
    theDialog->showTool("OpenSees@DesignSafe");
  });
  
  
  //
  // Add Tools to menu bar
  //
  
  QAction* menuAfter = nullptr;
  foreach (QAction *action, menuBar->actions())
  {
    // First check for an examples menu and if that does not exist put it before the help menu
    QString actionText = action->text();
    if(actionText.compare("&Examples") == 0)
    {
        menuAfter = action;
        break;
    }
    else if(actionText.compare("&Help") == 0)
      {
        menuAfter = action;
        break;
    }
  }
  menuBar->insertMenu(menuAfter, toolsMenu);
  //menuBar->addMenu(toolsMenu);
}



WorkflowAppEE_UQ::~WorkflowAppEE_UQ()
{
    // hack to get around a sometimes occuring seg fault
    // as some classes in destructur remove RV from the RVCOntainer
    // which may already have been destructed .. so removing so no destructor called

  //    QWidget *newUQ = new QWidget();
  //    theComponentSelection->swapComponent("RV",newUQ);
}

void WorkflowAppEE_UQ::replyFinished(QNetworkReply *pReply)
{
    return;
}


bool
WorkflowAppEE_UQ::outputToJSON(QJsonObject &jsonObjectTop) {

    //
    // get each of the main widgets to output themselves
    //
  
    bool result = true;
    QJsonObject apps;

    //
    // get each of the main widgets to output themselves to top 
    // and workflow widgets to outut appData to apps
    //

    // theGI
    QJsonObject jsonObjGenInfo;
    result = theGI->outputToJSON(jsonObjGenInfo);
    if (result == false)
        return result;
    jsonObjectTop["GeneralInformation"] = jsonObjGenInfo;

    // theRVs
    result = theRVs->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    // theEDP
    QJsonObject jsonObjectEDP;
    result = theEDP_Selection->outputToJSON(jsonObjectEDP);
    if (result == false)
        return result;
    jsonObjectTop["EDP"] = jsonObjectEDP;

    QJsonObject appsEDP;
    result = theEDP_Selection->outputAppDataToJSON(appsEDP);
    if (result == false)
        return result;
    apps["EDP"]=appsEDP;

    // theUQ
    result = theUQ_Selection->outputAppDataToJSON(apps);
    if (result == false)
        return result;
    
    result = theUQ_Selection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    // theSIM
    result = theSIM->outputAppDataToJSON(apps);
    if (result == false)
        return result;

    result = theSIM->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    // theAnalysis
    result = theAnalysisSelection->outputAppDataToJSON(apps);
    if (result == false)
        return result;

    result = theAnalysisSelection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

   // NOTE: Events treated differently, due to array nature of objects
    result = theEventSelection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    result = theEventSelection->outputAppDataToJSON(apps);
    if (result == false)
        return result;

    result = theRunWidget->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    // sy - to save results
    result = theResults->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    jsonObjectTop["Applications"]=apps;

    QJsonObject defaultValues;
    defaultValues["workflowInput"]=QString("scInput.json");    
    defaultValues["filenameAIM"]=QString("AIM.json");
    defaultValues["filenameEVENT"] = QString("EVENT.json");
    defaultValues["filenameSAM"]= QString("SAM.json");
    defaultValues["filenameEDP"]= QString("EDP.json");
    defaultValues["filenameSIM"]= QString("SIM.json");
    defaultValues["driverFile"]= QString("driver");
    defaultValues["filenameDL"]= QString("BIM.json");
    defaultValues["workflowOutput"]= QString("EDP.json");
    QJsonArray rvFiles, edpFiles;
    rvFiles.append(QString("AIM.json"));
    rvFiles.append(QString("SAM.json"));
    rvFiles.append(QString("EVENT.json"));
    rvFiles.append(QString("SIM.json"));
    edpFiles.append(QString("EDP.json"));
    defaultValues["rvFiles"]= rvFiles;
    defaultValues["edpFiles"]=edpFiles;
    jsonObjectTop["DefaultValues"]=defaultValues;

    return result;
}


void
WorkflowAppEE_UQ::processResults(QString &dirName){

  //
  // get results widget fr currently selected UQ option
  //

  theResults=theUQ_Selection->getResults();
  if (theResults == NULL) {
    this->errorMessage("FATAL - UQ option selected not returning results widget");
    return;
  }

  //
  // swap current results with existing one in selection & disconnect signals
  //

  QWidget *oldResults = theComponentSelection->swapComponent(QString("RES"), theResults);
  if (oldResults != NULL) {
    delete oldResults;
  }

  //
  // process results
  // 

  theResults->processResults(dirName);
  // theRunWidget->hide();
  theComponentSelection->displayComponent("RES");
}

void
WorkflowAppEE_UQ::clear(void)
{
    theGI->clear();
    theSIM->clear();
}

bool
WorkflowAppEE_UQ::inputFromJSON(QJsonObject &jsonObject)
{
    //
    // get each of the main widgets to input themselves
    //

    if (jsonObject.contains("GeneralInformation")) {
        QJsonObject jsonObjGeneralInformation = jsonObject["GeneralInformation"].toObject();
        if (theGI->inputFromJSON(jsonObjGeneralInformation) == false) {
            this->errorMessage("EE_UQ: failed to read GeneralInformation");
        }
    } else {
        this->errorMessage("EE_UQ: failed to find GeneralInformation");
        return false;
    }

    if (jsonObject.contains("Applications")) {

        QJsonObject theApplicationObject = jsonObject["Applications"].toObject();

        // note: Events is different because the object is an Array
        if (theApplicationObject.contains("Events")) {
            //  QJsonObject theObject = theApplicationObject["Events"].toObject(); it is null object, actually an array
            if (theEventSelection->inputAppDataFromJSON(theApplicationObject) == false) {
                this->errorMessage("EE_UQ: failed to read Event Application");
            }

        } else {
            this->errorMessage("EE_UQ: failed to find Event Application");
            return false;
        }

        if (theUQ_Selection->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("EE_UQ: failed to read UQ application");

        if (theSIM->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("EE_UQ: failed to read SIM application");
    
        if (theAnalysisSelection->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("EE_UQ: failed to read FEM application");

        if (theApplicationObject.contains("EDP")) {
            QJsonObject theObject = theApplicationObject["EDP"].toObject();
            if (theEDP_Selection->inputAppDataFromJSON(theObject) == false) {
                this->errorMessage("EE_UQ: failed to read EDP application");
            }
        } else {
            this->errorMessage("EE_UQ: failed to find EDP application");
            return false;
        }

    } else
        return false;

    /*
    ** Note to me - RVs and Events treated differently as both use arrays .. rethink API!
    */

    theEventSelection->inputFromJSON(jsonObject);
    theRVs->inputFromJSON(jsonObject);
    theRunWidget->inputFromJSON(jsonObject);

    if (jsonObject.contains("EDP")) {
        QJsonObject edpObj = jsonObject["EDP"].toObject();
        if (theEDP_Selection->inputFromJSON(edpObj) == false)
            this->errorMessage("EE_UQ: failed to read EDP data");
    } else {
        this->errorMessage("EE_UQ: failed to find EDP data");
        return false;
    }


    if (theUQ_Selection->inputFromJSON(jsonObject) == false)
       this->errorMessage("EE_UQ: failed to read UQ Method data");

    if (theAnalysisSelection->inputFromJSON(jsonObject) == false)
        this->errorMessage("EE_UQ: failed to read FEM Method data");

    if (theSIM->inputFromJSON(jsonObject) == false)
        this->errorMessage("EE_UQ: failed to read SIM Method data");

    // sy - to display results
    auto* theNewResults = theUQ_Selection->getResults();

    if (theNewResults->inputFromJSON(jsonObject) == false)
        this->errorMessage("EE_UQ: failed to read RES Method data");
    theResults->setResultWidget(theNewResults);

    this->statusMessage("Done Loading File");
    
    return true;
}


void
WorkflowAppEE_UQ::onRunButtonClicked() {
    theRunWidget->hide();
    theRunWidget->setMinimumWidth(this->width()*0.5);
    theRunWidget->showLocalApplication();
    GoogleAnalytics::ReportLocalRun();
}

void
WorkflowAppEE_UQ::onRemoteRunButtonClicked(){
    this->errorMessage("");

    bool loggedIn = theRemoteService->isLoggedIn();

    if (loggedIn == true) {
        theRunWidget->hide();
        theRunWidget->setMinimumWidth(this->width()*0.5);
        theRunWidget->showRemoteApplication();

    } else {
        errorMessage("ERROR - You Need to Login");
    }

    GoogleAnalytics::ReportDesignSafeRun();
}

void
WorkflowAppEE_UQ::onRemoteGetButtonClicked(){

    this->errorMessage("");

    bool loggedIn = theRemoteService->isLoggedIn();

    if (loggedIn == true) {

        theJobManager->hide();
        theJobManager->updateJobTable("");
        theJobManager->show();

    } else {
        errorMessage("ERROR - You Need to Login");
    }
}

void
WorkflowAppEE_UQ::onExitButtonClicked(){

}

void
WorkflowAppEE_UQ::setUpForApplicationRun(QString &workingDir, QString &subDir) {

    errorMessage("");

    //
    // create temporary directory in working dir
    // and copy all files needed to this directory by invoking copyFiles() on app widgets
    //

    QString tmpDirName = QString("tmp.SimCenter");
    QDir workDir(workingDir);

    QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
    QDir destinationDirectory(tmpDirectory);

    if(destinationDirectory.exists()) {
      if (isSafeToRemoveRecursivily(tmpDirectory))
	destinationDirectory.removeRecursively();
    }
    
    destinationDirectory.mkpath(tmpDirectory);
    QString templateDirectory  = destinationDirectory.absoluteFilePath(subDir);
    destinationDirectory.mkpath(templateDirectory);

    // copyPath(path, tmpDirectory, false);
    //theSIM->copyFiles(templateDirectory);
    if (theSIM->copyFiles(templateDirectory) == false) {
      errorMessage("Workflow Failed to start as SIM failed in copyFiles");
      return;
    }
    if (theEventSelection->copyFiles(templateDirectory) == false) {
      errorMessage("Workflow Failed to start as EVENT failed in copyFiles");
      return;
    }
    theAnalysisSelection->copyFiles(templateDirectory);
    theUQ_Selection->copyFiles(templateDirectory);
    theEDP_Selection->copyFiles(templateDirectory);

    //
    // in new templatedir dir save the UI data into scInput.json file (same result as using saveAs)
    // NOTE: we append object workingDir to this which points to template dir
    //

    QString inputFile = templateDirectory + QDir::separator() + tr("scInput.json");

    QFile file(inputFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        //errorMessage();
        return;
    }
    QJsonObject json;
    if (this->outputToJSON(json) == false) {
        this->errorMessage("WorkflowApp - failed in outputToJson");
        return;
    }
    json["runDir"]=tmpDirectory;
    json["WorkflowType"]="Building Simulation";

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    
    //
    // ouput citation
    //
    
    QJsonObject citations;
    QString citeFile = templateDirectory + QDir::separator() + tr("please_cite.json");    
    // QString citeFile = destinationDirectory.filePath("please_cite.json"); // file getting deleted

    this->createCitation(citations, citeFile);


    statusMessage("SetUp Done .. Now starting application");
    emit setUpForApplicationRunDone(tmpDirectory, inputFile);
}

int
WorkflowAppEE_UQ::loadFile(QString &fileName){

    errorMessage("");

    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        this->errorMessage(QString("Could Not Open File: ") + fileName);
        return -1;
    }

    //
    // place contents of file into json object
    //

    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    //Resolve absolute paths from relative ones
    QFileInfo fileInfo(fileName);
    SCUtils::ResolveAbsolutePaths(jsonObj, fileInfo.dir());
    
    // close file
    file.close();

    //
    // clear current and input from new JSON
    //

    this->clear();
    bool result = this->inputFromJSON(jsonObj);
    if (result == true)
      return 0;
    else
      return -1;
}

int
WorkflowAppEE_UQ::getMaxNumParallelTasks() {
    return theUQ_Selection->getNumParallelTasks();
}

int
WorkflowAppEE_UQ::createCitation(QJsonObject &citation, QString citeFile) {

  QString cit("{\"EE-UQ\": { \"citations\": [{\"citation\": \"Frank McKenna, Kuanshi Zhong, Michael Gardner, Adam Zsarnoczay, Sang-ri Yi, Aakash Bangalore Satish, Charles Wang, Amin Pakzad, & Wael Elhaddad. (2024). NHERI-SimCenter/quoFEM: Version 4.0.0 (v4.0.0). Zenodo. https://doi.org/10.5281/zenodo.13324130\",\"description\": \"This is the overall tool reference used to indicate the version of the tool.\"},{\"citation\": \"Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, Ahsan Kareem, Wael Elhaddad, Laura Lowes, Matthew J. Schoettler, and Sanjay Govindjee (2020) A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment. Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706\",\"description\": \" This marker paper describes the SimCenter application framework, which was designed to simulate the impacts of natural hazards on the built environment. It is a necessary attribute for publishing work resulting from the use of SimCenter tools, software, and datasets.\"}]}}");

  QJsonDocument docC = QJsonDocument::fromJson(cit.toUtf8());
  if(!docC.isNull()) {
    if(docC.isObject()) {
      citation = docC.object();        
    }  else {
      qDebug() << "WorkflowAppEE_UQ citation text is not valid JSON: \n" << cit << endl;
    }
  }
  
  theSIM->outputCitation(citation);
  theEventSelection->outputCitation(citation);
  theAnalysisSelection->outputCitation(citation);
  theUQ_Selection->outputCitation(citation);
  theEDP_Selection->outputCitation(citation);
   
  // write the citation to a citeFile if provided
  
  if (!citeFile.isEmpty()) {
    
    QFile file(citeFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
      errorMessage(QString("writeCitation - could not open file") + citeFile);
      progressDialog->hideProgressBar();
      return 0;
    }

    QJsonDocument doc(citation);
    file.write(doc.toJson());
    file.close();
  }
  
  return 0;    
}

