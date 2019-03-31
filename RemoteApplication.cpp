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

// Purpose: a widget for managing submiited jobs by WorkflowApp tool
//  - allow for refresh of status, deletion of submitted jobs, and download of results from finished job

#include "RemoteApplication.h"
#include <RemoteService.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>

//#include <AgaveInterface.h>
#include <QDebug>
#include <QDir>
#include <QUuid>
#include <QFileDialog>
#include <ZipUtils.h>

RemoteApplication::RemoteApplication(QString name, RemoteService *theService, QWidget *parent)
: Application(parent), theRemoteService(theService)
{
    workflowScriptName = name;
    shortDirName = workflowScriptName;
    //shortDirName = name.chopped(3); // remove .py
    shortDirName.chop(3);

    QGridLayout *layout = new QGridLayout();
    QLabel *nameLabel = new QLabel();

    nameLabel->setText(QString("job Name:"));
    layout->addWidget(nameLabel, 0,0);

    nameLineEdit = new QLineEdit();
    nameLineEdit->setToolTip(tr("A meaningful name to provide for you to remember run later (days and weeks from now)"));
    layout->addWidget(nameLineEdit,0,1);

    QLabel *numCPU_Label = new QLabel();
    numCPU_Label->setText(QString("Num Nodes:"));

    layout->addWidget(numCPU_Label,1,0);

    numCPU_LineEdit = new QLineEdit();
    numCPU_LineEdit->setText("1");
    numCPU_LineEdit->setToolTip(tr("Total # of nodes to use (each node has many cores)"));
    layout->addWidget(numCPU_LineEdit,1,1);

    QLabel *numProcessorsLabel = new QLabel();
    numProcessorsLabel->setText(QString("Total # Processes:"));

    layout->addWidget(numProcessorsLabel,2,0);

    numProcessorsLineEdit = new QLineEdit();
    numProcessorsLineEdit->setText("32");
        numProcessorsLineEdit->setToolTip(tr("Total # of Processes to Start"));
    layout->addWidget(numProcessorsLineEdit,2,1);

    QLabel *runtimeLabel = new QLabel();
    runtimeLabel->setText(QString("Max Run Time:"));
    layout->addWidget(runtimeLabel,3,0);

    runtimeLineEdit = new QLineEdit();
    runtimeLineEdit->setText("00:20:00");
    runtimeLineEdit->setToolTip(tr("Run time Limit on running Job hours:Min:Sec. Job will be stopped if while running it exceeds this"));
    layout->addWidget(runtimeLineEdit,3,1);

    QLabel *appNameLabel = new QLabel();
    appNameLabel->setText("App Name");
    layout->addWidget(appNameLabel,4,0);

    appLineEdit = new QLineEdit();
    appLineEdit->setText("simcenter-dakota-1.0.0u1");
    appLineEdit->setToolTip(tr("Name of Agave App to run. Do not edit unless you really know what you are doing!"));
    //appLineEdit->setText("Dakota-6.6.0.0u1");
    layout->addWidget(appLineEdit,4,1);

    //Working Directory
    QLabel *workingDirLabel = new QLabel();
    workingDirLabel->setText(QString("Working Directory:"));
    layout->addWidget(workingDirLabel,5,0);

    workingDirName = new QLineEdit();
    QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    workingDirName->setText(workingDir.filePath(QCoreApplication::applicationName() + "/RemoteWorkDir"));
    workingDirName->setToolTip(tr("Location on your system we need to use to store tmp files"));
    layout->addWidget(workingDirName,5,1);

    QPushButton *workDirButton = new QPushButton();
    workDirButton->setText("Browse");
    workDirButton->setToolTip(tr("Select the Working Directory"));
    layout->addWidget(workDirButton,5,2);

    //Local Workflow Applications Directory
    QLabel *appDirLabel = new QLabel();
    appDirLabel->setText(QString("Local Applications Directory:"));
    layout->addWidget(appDirLabel,6,0);

    localAppDirName = new QLineEdit();
    localAppDirName->setText(QCoreApplication::applicationDirPath());
    localAppDirName->setToolTip(tr("Location on your system where the SimCenter workflow applications exist. Only edit if you know what you are doing."));
    layout->addWidget(localAppDirName,6,1);

    QPushButton *localAppsDirButton = new QPushButton();
    localAppsDirButton->setText("Browse");
    localAppsDirButton->setToolTip(tr("Select the Local Workflow Applications Directory"));
    layout->addWidget(localAppsDirButton,6,2);


    QLabel *appDirLabel1 = new QLabel();
    appDirLabel1->setText(QString("Remote Applications Directory:"));
    layout->addWidget(appDirLabel1,7,0);

    remoteAppDirName = new QLineEdit();
    remoteAppDirName->setText("/home1/00477/tg457427/SimCenter/EE-UQ-V1.1");
    remoteAppDirName->setToolTip(tr("Location on TACC Stampede 2 where the SimCenter workflow applications exist(For Advanced Users)"));

    layout->addWidget(remoteAppDirName,7,1);

    pushButton = new QPushButton();
    pushButton->setText("Submit");
    pushButton->setToolTip(tr("Press to launch job on remote machine. After pressing, window closes when Job Starts"));
    layout->addWidget(pushButton,8,1);

    this->setLayout(layout);

    //
    // set up connections
    //

    //    connect(pushButton,SIGNAL(clicked()), this, SLOT(pushButtonClicked()));

    // on login from interface to set up homeDirPath
    //    connect(theService,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));
    connect(this,SIGNAL(getHomeDirCall()),theService,SLOT(getHomeDirPathCall()));
    connect(theService,SIGNAL(getHomeDirPathReturn(QString)), this, SLOT(getHomeDirReturned(QString)));

    // to start job need to connect uploadDir and start job
    connect(this,SIGNAL(uploadDirCall(const QString &,const QString &)), theService, SLOT(uploadDirectoryCall(const QString &,const QString &)));
    connect(theService, SIGNAL(uploadDirectoryReturn(bool)), this, SLOT(uploadDirReturn(bool)));
    connect(this,SIGNAL(startJobCall(QJsonObject)),theService,SLOT(startJobCall(QJsonObject)));
    connect(theService,SIGNAL(startJobReturn(QString)), this, SLOT(startJobReturn(QString)));

    //Automatically changing to forward slash
    connect(workingDirName, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            workingDirName->setText(newValue.replace('\\','/'));
    });

    connect(localAppDirName, &QLineEdit::textChanged, this, [this](QString newValue){
        if (newValue.contains('\\'))
            localAppDirName->setText(newValue.replace('\\','/'));
    });

    //Browse buttons
    connect(workDirButton, &QPushButton::clicked, this, [this](){
        QString existingDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

        if(QDir(workingDirName->text()).exists())
            existingDir = workingDirName->text();

        QString selectedDir = QFileDialog::getExistingDirectory(this,
                                                                tr("Select Local Working Directory for Cloud Simulation"),
                                                                existingDir,
                                                                QFileDialog::ShowDirsOnly);

        if(!selectedDir.isEmpty())
            workingDirName->setText(selectedDir);
    });

    connect(localAppsDirButton, &QPushButton::clicked, this, [this](){
        QString existingDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

        if(QDir(localAppDirName->text()).exists())
            existingDir = localAppDirName->text();

        QString selectedDir = QFileDialog::getExistingDirectory(this,
                                                                tr("Select Local SimCenter Workflow Applications Directory for Cloud Simulation"),
                                                                existingDir,
                                                                QFileDialog::ShowDirsOnly);

        if(!selectedDir.isEmpty())
            localAppDirName->setText(selectedDir);
    });

    //
    // set up connections
    //

    /*
    this->setStyleSheet("QComboBox {background: #FFFFFF;} \
  QGroupBox {font-weight: bold;}\
  QLineEdit {background-color: #FFFFFF; border: 2px solid darkgray;} \
  QTabWidget::pane {background-color: #ECECEC; border: 1px solid rgb(239, 239, 239);}");
  */

    connect(pushButton,SIGNAL(clicked()), this, SLOT(onRunButtonPressed()));
}

bool
RemoteApplication::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["localAppDir"]=localAppDirName->text();
    jsonObject["remoteAppDir"]=remoteAppDirName->text();
    jsonObject["remoteAppWorkingDir"]=remoteAppDirName->text(); // we use this one so that default not overwritten if run local
    jsonObject["workingDir"]=workingDirName->text();
    jsonObject["runType"]=QString("HPC");

    return true;
}

bool
RemoteApplication::inputFromJSON(QJsonObject &dataObject) {

    if (dataObject.contains("localAppDir")) {
        QJsonValue theName = dataObject["localAppDir"];
        if(QDir(theName.toString()).exists())
            localAppDirName->setText(theName.toString());
        else
            localAppDirName->setText(QCoreApplication::applicationDirPath());
    } else
        return false;

    if (dataObject.contains("remoteAppWorkingDir")) {
        QJsonValue theName = dataObject["remoteAppWorkingDir"];
        remoteAppDirName->setText(theName.toString());
    }

    if (dataObject.contains("workingDir")) {
        QJsonValue theName = dataObject["workingDir"];
        if(QDir(theName.toString()).exists())
            workingDirName->setText(theName.toString());
        else {
            QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
            workingDirName->setText(workingDir.filePath(QCoreApplication::applicationName() + "/RemoteWorkDir"));
        }
    } else
        return false;

    return true;
}




void
RemoteApplication::onRunButtonPressed(void)
{
    int ok = 0;
    QString workingDir = workingDirName->text();
    QDir dirWork(workingDir);
    if (!dirWork.exists())
        if (!dirWork.mkpath(workingDir)) {
            emit sendErrorMessage(QString("Could not create Working Dir: ") + workingDir + QString(" . Try using an existing directory or make sure you have permission to create the working directory."));
            return;
        }

   QString appDir = localAppDirName->text();
   QDir dirApp(appDir);
   if (!dirApp.exists()) {
       emit sendErrorMessage(QString("The application directory, ") + appDir +QString(" specified does not exist!"));
       return;
   }

    QString templateDir("templatedir");

    emit setupForRun(workingDir, templateDir);
}


//
// now use the applications Workflow Application script to run dakota and produce output files:
//    dakota.in dakota.out dakotaTab.out dakota.err
//

bool
RemoteApplication::setupDoneRunApplication(QString &tmpDirectory, QString &inputFile) {

    QString appDir = localAppDirName->text();

    qDebug() << "REMOTEAPP: setupDone " << tmpDirectory << " " << inputFile << " " << appDir;
    QString pySCRIPT;

    QDir scriptDir(appDir);
    scriptDir.cd("applications");
    scriptDir.cd("Workflow");
    pySCRIPT = scriptDir.absoluteFilePath(workflowScriptName);
    QFileInfo check_script(pySCRIPT);
    // check if file exists and if yes: Is it really a file and no directory?
    if (!check_script.exists() || !check_script.isFile()) {
        qDebug() << "NO SCRIPT FILE: " << pySCRIPT;
        return false;
    }

    QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
    QFileInfo check_registry(registryFile);
    if (!check_registry.exists() || !check_registry.isFile()) {
         qDebug() << "NO REGISTRY FILE: " << registryFile;
        return false;
    }

    qDebug() << "SCRIPT: " << pySCRIPT;
    qDebug() << "REGISTRY: " << registryFile;

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
    QStringList args{pySCRIPT, "set_up",inputFile,registryFile};
    proc->execute("python",args);

    /*
#ifdef Q_OS_WIN
    QString command = QString("python ") + pySCRIPT + QString(" ") + " set_up " + QString(" ") + inputFile  + QString(" ") + registryFile;
    qDebug() << "PYTHON COMMAND: " << command;

    proc->execute("cmd", QStringList() << "/C" << command);

#else
    QString command = QString("source $HOME/.bash_profile; python ") + pySCRIPT + QString(" set_up ") + inputFile + QString(" ") +
            registryFile;

    proc->execute("bash", QStringList() << "-c" <<  command);
    qDebug() << "PYTHON COMMAND: " << command;

#endif
    */
    proc->waitForStarted();



    //
    // in tmpDirectory we will zip up current template dir and then remove before sending (doone to reduce number of sends)
    //

    QDir templateDir(tmpDirectory);
    templateDir.cd("templatedir");
    QString templateDIR = templateDir.absolutePath();

#ifdef Q_OS_WIN
    templateDir.rename("workflow_driver.bat","workflow_driver");
#endif

    QFileInfo check_workflow(templateDir.absoluteFilePath("workflow_driver"));
    if (!check_workflow.exists() || !check_workflow.isFile()) {
        emit sendErrorMessage(("Local Failure Setting up Dakota"));
        qDebug() << "Local Failure Setting Up Dakota ";
        return false;
    }
    templateDir.cdUp();

    QString zipFile(templateDir.absoluteFilePath("templatedir.zip"));
    qDebug() << "ZIP FILE: " << zipFile;
    qDebug() << "DIR TO ZIP: " << templateDIR;
    QDir tmpDir(templateDIR);

    ZipUtils::ZipFolder(tmpDir, zipFile);
    //ZipUtils::ZipFolder(QDir(templateDIR), zipFile);

    QDir dirToRemove(templateDIR);
    templateDir.cd("templatedir");
    templateDir.removeRecursively();

    //
    // now upload files to remote local
    //

    // rename dir so unique at DesignSafe
    QUuid uniqueName = QUuid::createUuid();
    QString strUnique = uniqueName.toString();
    strUnique = strUnique.mid(1,36);
    QString newName = QString("tmp.SimCenter") + strUnique;

    QDir theDirectory(tmpDirectory);
    theDirectory.cdUp();
    if (theDirectory.rename("tmp.SimCenter",newName) != true) {
        emit sendErrorMessage(QString("Could not rename directory to ") + newName);
        return false;
    }

    qDebug() << "newName: " << newName;
    tempDirectory = theDirectory.absoluteFilePath(newName);

    theDirectory.cd(newName);
    QString dirName = theDirectory.dirName();
    
    QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
    pushButton->setEnabled(false);

    emit uploadDirCall(tempDirectory, remoteHomeDirPath);

    return 0;
}

// this slot is invoked on return from uploadDirectory signal in pushButtonClicked slot

void
RemoteApplication::uploadDirReturn(bool result)
{
    if (result == true) {

      //
      // create the json needed to run the remote application
      //

      QJsonObject job;
      
      pushButton->setDisabled(true);
      
      job["name"]=shortDirName + nameLineEdit->text();
      int nodeCount = numCPU_LineEdit->text().toInt();
      int numProcessorsPerNode = numProcessorsLineEdit->text().toInt();
      job["nodeCount"]=nodeCount;
      job["processorsPerNode"]=nodeCount*numProcessorsPerNode;
      job["requestedTime"]=runtimeLineEdit->text();
      
      // defaults (possibly from a parameters file)
      //Dakota-6.6.0.0u1
      //job["appId"]="dakota-6.6.0";
      //job["appId"]="Dakota-6.6.0.0u1";
      
      job["appId"]=appLineEdit->text();
      job["memoryPerNode"]= "1GB";
      job["archive"]="true";
      job["archiveSystem"]="designsafe.storage.default";
      
      QJsonObject parameters;
      parameters["inputFile"]="dakota.in";
      parameters["outputFile"]="dakota.out";
      parameters["errorFile"]="dakota.err";
      parameters["driverFile"]="workflow_driver";
      parameters["modules"]="petsc";
      job["parameters"]=parameters;
      
      QDir theDirectory(tempDirectory);
      QString dirName = theDirectory.dirName();
      
      QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
      
      QJsonObject inputs;
      
      inputs["inputDirectory"]=remoteDirectory;
      job["inputs"]=inputs;
      
      // disable the button while the job is being uploaded and started
      pushButton->setEnabled(false);

      qDebug() << "JOBS_SUBMIT: " << job;

      //
      // start the remote job
      //

      emit startJobCall(job);
      
      // now remove the tmp directory
      theDirectory.removeRecursively();
    }
}

void
RemoteApplication::attemptLoginReturn(bool ok) {
    if (ok == true) {
        emit getHomeDirCall();
    }
}


void
RemoteApplication::getHomeDirReturned(QString path){
    remoteHomeDirPath = path;
}

void
RemoteApplication::startJobReturn(QString result) {
   pushButton->setEnabled(true);
   emit successfullJobStart();
}

void
RemoteApplication::setNumTasks(int numTasks) {
    if (numTasks < 32)
        numProcessorsLineEdit->setText(QString::number(numTasks));
}

