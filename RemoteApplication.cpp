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

// Purpose: a widget for managing submiited jobs by uqFEM tool
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


RemoteApplication::RemoteApplication(RemoteService *theService, QWidget *parent)
: Application(parent), theRemoteService(theService)
{
    QGridLayout *layout = new QGridLayout();
    QLabel *nameLabel = new QLabel();

    nameLabel->setText(QString("job Name:"));
    layout->addWidget(nameLabel, 0,0);

    nameLineEdit = new QLineEdit();
    layout->addWidget(nameLineEdit,0,1);

    QLabel *numCPU_Label = new QLabel();
    numCPU_Label->setText(QString("num Nodes:"));
    layout->addWidget(numCPU_Label,1,0);

    numCPU_LineEdit = new QLineEdit();
    numCPU_LineEdit->setText("1");
    layout->addWidget(numCPU_LineEdit,1,1);

    QLabel *numProcessorsLabel = new QLabel();
    numProcessorsLabel->setText(QString("num Processors per Node:"));
    layout->addWidget(numProcessorsLabel,2,0);

    numProcessorsLineEdit = new QLineEdit();
    numProcessorsLineEdit->setText("32");
    layout->addWidget(numProcessorsLineEdit,2,1);

    QLabel *runtimeLabel = new QLabel();
    runtimeLabel->setText(QString("max Run Time:"));
    layout->addWidget(runtimeLabel,3,0);

    runtimeLineEdit = new QLineEdit();
    runtimeLineEdit->setText("00:20:00");
    layout->addWidget(runtimeLineEdit,3,1);

    QLabel *appNameLabel = new QLabel();
    appNameLabel->setText("App Name");
    layout->addWidget(appNameLabel,4,0);

    appLineEdit = new QLineEdit();
    appLineEdit->setText("dakota-6.6.0u1");
    //appLineEdit->setText("Dakota-6.6.0.0u1");
    layout->addWidget(appLineEdit,4,1);

    QLabel *workingDirLabel = new QLabel();
    workingDirLabel->setText(QString("Working Dir Location:"));
    layout->addWidget(workingDirLabel,5,0);

    workingDirName = new QLineEdit();
    workingDirName->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    layout->addWidget(workingDirName,5,1);

    QLabel *appDirLabel = new QLabel();
    appDirLabel->setText(QString("Local App Dir Location:"));
    layout->addWidget(appDirLabel,6,0);

    localAppDirName = new QLineEdit();
    localAppDirName->setText(QCoreApplication::applicationDirPath());
    layout->addWidget(localAppDirName,6,1);

    QLabel *appDirLabel1 = new QLabel();
    appDirLabel1->setText(QString("Remote App Dir Location:"));
    layout->addWidget(appDirLabel1,7,0);

    remoteAppDirName = new QLineEdit();
    remoteAppDirName->setText("/home/tg457427/SimCenter-EE-UQ");
    layout->addWidget(remoteAppDirName,7,1);

    pushButton = new QPushButton();
    pushButton->setText("Submit");
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


    //
    // set up connections
    //

    connect(pushButton,SIGNAL(clicked()), this, SLOT(onRunButtonPressed()));
}

bool
RemoteApplication::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["localAppDir"]=localAppDirName->text();
    jsonObject["remoteAppDir"]=remoteAppDirName->text();
    jsonObject["remoteAppWorkingDirDir"]=remoteAppDirName->text(); // we use this one so that default not overwritten if run local
    jsonObject["workingDir"]=workingDirName->text();

    return true;
}

bool
RemoteApplication::inputFromJSON(QJsonObject &dataObject) {

    if (dataObject.contains("localAppDir")) {
        QJsonValue theName = dataObject["localAppDir"];
        localAppDirName->setText(theName.toString());
    } else
        return false;

    if (dataObject.contains("remoteAppWorkingDirDir")) {
        QJsonValue theName = dataObject["remoteAppWorkingDir"];
        remoteAppDirName->setText(theName.toString());
    }

    if (dataObject.contains("workingDir")) {
        QJsonValue theName = dataObject["workingDir"];
        workingDirName->setText(theName.toString());
    } else
        return false;

    return true;
}




void
RemoteApplication::onRunButtonPressed(void)
{
    qDebug() << "RemoteApplication::onRunButtonClicked()";

    int ok = 0;
    QString workingDir = workingDirName->text();
    QDir dirWork(workingDir);
    if (!dirWork.exists())
        if (!dirWork.mkdir(workingDir)) {
            emit sendErrorMessage(QString("Could not create Working Dir: ") + workingDir);
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
// now use the applications Workflow Application EE-UQ.py  to run dakota and produce output files:
//    dakota.in dakota.out dakotaTab.out dakota.err
//

bool
RemoteApplication::setupDoneRunApplication(QString &tmpDirectory, QString &inputFile) {

    QString appDir = localAppDirName->text();

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
    QString command = QString("python ") + pySCRIPT + QString(" ") + tDirectory + QString(" ") + tmpDirectory  + QString(" runningRemote");
    qDebug() << command;
    proc->execute("cmd", QStringList() << "/C" << command);
    //   proc->start("cmd", QStringList(), QIODevice::ReadWrite);

#else
    QString command = QString("source $HOME/.bash_profile; python ") + pySCRIPT + QString(" set_up ") + inputFile + QString(" ") +
            registryFile;

    proc->execute("bash", QStringList() << "-c" <<  command);

    qInfo() << command;

#endif
    proc->waitForStarted();

    //
    // now upload files to remote local
    //
    tempDirectory = tmpDirectory;

    QDir theDirectory(tmpDirectory);
    QString dirName = theDirectory.dirName();
    
    QString remoteDirectory = remoteHomeDirPath + QString("/") + dirName;
    
    pushButton->setEnabled(false);

    emit uploadDirCall(tmpDirectory, remoteHomeDirPath);
}

// this slot is invoked on return from uploadDirectory signal in pushButtonClicked slot

void
RemoteApplication::uploadDirReturn(bool result)
{
    qDebug() << "RemoteApp:: uploadDirReturned";

    if (result == true) {

      //
      // create the json needed to run the remote application
      //

      QJsonObject job;
      
      pushButton->setDisabled(true);
      
      job["name"]=QString("uqFEM:") + nameLineEdit->text();
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
      parameters["driverFile"]="fem_driver";
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
      //theDirectory.removeRecursively();
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
   this->hide();
}
