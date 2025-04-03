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

#include "IstanbulSingleSite.h"
#include <SC_DoubleLineEdit.h>
#include <SC_IntLineEdit.h>
#include <SC_DirEdit.h>
#include <SC_ComboBox.h>

#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QSplitter>
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QStringList>
#include <SimCenterPreferences.h>
#include <ModularPython.h>
#include <RunPythonInThread.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QScreen>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>


IstanbulSingleSite::IstanbulSingleSite(QWidget *parent)
  :SimCenterAppWidget(parent), count(0), ok(false), downloadedMotions(false), motionsDownloading(false)
{

  this->setWindowTitle("Splitting Widgets Example");
  this->setGeometry(100, 100, 600, 400);
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  QWidget *leftWidget = new QWidget();
  QGridLayout *leftLayout = new QGridLayout(leftWidget);
  leftLayout->setAlignment(Qt::AlignTop);
  // this->setLayout(theLayout);
  QGroupBox *inputParameters = new QGroupBox("Input Parameters");

  QGridLayout *theLayout = new QGridLayout(inputParameters);
  this->setLayout(theLayout);

  theLayout->addWidget(new QLabel("grid type"),0,0);
  QStringList listGrids; listGrids << "All";
  gridType = new SC_ComboBox("gridType",listGrids);
  theLayout->addWidget(gridType, 0, 1);
  
  theLayout->addWidget(new QLabel("num Motions"),0,2);
  numRealizations = new SC_IntLineEdit("numRealizations",1);
  theLayout->addWidget(numRealizations,0,3);
  
  theLayout->addWidget(new QLabel("latitude"),2,0);
  lat = new SC_DoubleLineEdit("latitude",40.9940);
  theLayout->addWidget(lat,2,1);
  theLayout->addWidget(new QLabel("longitude"),2,2);
  lng = new SC_DoubleLineEdit("longitude",28.8990);
  theLayout->addWidget(lng,2,3);




  QString dirPath = SimCenterPreferences::getInstance()->getLocalWorkDir() + QDir::separator() + "Istanbul";
  theLayout->addWidget(new QLabel("tmp Directory"),3,0);  
  tmpLocation = new SC_DirEdit("tmpLocation");
  tmpLocation->setDirName(dirPath);
  theLayout->addWidget(tmpLocation,3,1,1,2);

  getMotions = new QPushButton("Get Motions");
  theLayout->addWidget(getMotions, 4,1,1,3);
  connect(getMotions, &QPushButton::clicked, this, [=](){
    this->downloadMotions();
  });
  QLabel *citation = new QLabel("Zhang, W., J. Crempien, K. Zhong, P. Chen, P. Arduino, E. Taciroglu. (2023), A suite of 57 broadband physics-based ground motion simulations for the Istanbul region, in Regional-scale physics-based ground motion simulation for Istanbul, Turkey. DesignSafe-CI.");
  citation->setWordWrap(true);
  theLayout->addWidget(citation,5,0,1,4);
  theLayout->setRowStretch(6,1);
  theLayout->setColumnStretch(1,1);
  theLayout->setColumnStretch(3,1); 
  theLayout->setColumnStretch(4,1);   
  leftLayout->addWidget(inputParameters,0,0);  

  QGroupBox *mapboxIstanbul = new QGroupBox("Grid Location");

  // ===========================================================
  // creating right widget
  // ===========================================================
  QWidget *rightWidget = new QWidget();

  QGridLayout *rightLayout = new QGridLayout(rightWidget);

  QGroupBox *mapbox = new QGroupBox("Grid Map");
  QGridLayout *maplayout = new QGridLayout(mapbox);

  QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
  QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

  // webView1->page()->setDevToolsPage(new QWebEnginePage());
  webView1 = new QWebEngineView();
  QString appDir = SimCenterPreferences::getInstance()->getAppDir();
  QString mappscript = appDir + QDir::separator() + "applications" + QDir::separator()
    + "createEVENT" + QDir::separator() + "Istanbul" + QDir::separator();
  // webView1->setUrl(QUrl("https://www.google.com/maps"));
  webView1->load(QUrl::fromLocalFile(mappscript+"Istanbul_AllSites_grid.html"));
  webView1->show();
  maplayout->addWidget(webView1,0,0);
  rightLayout->addWidget(mapbox,0,0);



  leftWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  rightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainLayout->addWidget(leftWidget);
  mainLayout->addWidget(rightWidget);



  mainLayout->addWidget(leftWidget);
  mainLayout->addWidget(rightWidget);
}

IstanbulSingleSite::~IstanbulSingleSite()
{

  
  
  
}

void
IstanbulSingleSite::downloadMotions(void)
{
  qDebug() << "IstanbulSingleSite::downloadMotions called";
  
  // pop up a window to ask for username and password
  QDialog *dialog = new QDialog();
  dialog->setWindowTitle("Login to Tapis");
  QVBoxLayout *layout = new QVBoxLayout(dialog);
  QLineEdit *usernameEdit = new QLineEdit(dialog);
  usernameEdit->setPlaceholderText("Username");
  layout->addWidget(usernameEdit);
  QLineEdit *passwordEdit = new QLineEdit(dialog);
  passwordEdit->setPlaceholderText("Password");
  passwordEdit->setEchoMode(QLineEdit::Password);
  layout->addWidget(passwordEdit);
  QPushButton *loginButton = new QPushButton("Login", dialog);
  layout->addWidget(loginButton);
  dialog->setLayout(layout);
  connect(loginButton, &QPushButton::clicked, [=]() {
      // check if the username and password are not empty
      if (usernameEdit->text().isEmpty() || passwordEdit->text().isEmpty()) {
          errorMessage("Error: Username and password cannot be empty");
          return;
      }
      // close the dialog
      dialog->accept();
  });
  
  // make the dialog appear in the center of the screen instead of the top left corner
  // keep the dialog dimensions the same
  QScreen *screen = qApp->primaryScreen();
  QRect screenGeometry = screen->availableGeometry();
  int x = (screenGeometry.width() - dialog->width()) / 2;
  int y = (screenGeometry.height() - dialog->height()) / 2;
  dialog->move(x, y);
  dialog->exec();
  
  // save the username and password
  username = usernameEdit->text();
  password = passwordEdit->text();
  
  // If user canceled the dialog, return
  if (username.isEmpty() || password.isEmpty()) {
    return;
  }

  //
  // get tmp directory to store motions, if it exists remove
  //
  QString destDir = tmpLocation->getDirName();
  QDir destinationDirectory(destDir);
  
  if(destinationDirectory.exists()) {
    destinationDirectory.removeRecursively();
    destinationDirectory.mkpath(destDir);    
  } else
    destinationDirectory.mkpath(destDir);

  if (!destinationDirectory.exists()) {
    errorMessage(QString("IstanbulSingleSite SHOULD NOT HAPPEN BUT NO ") + destDir);
    return;
  }
  
  //
  // get all inputs
  //
  
  double latitude = lat->getDouble();
  double longitude = lng->getDouble();
  QString currentGrid=gridType->currentText();
  int numMotion = numRealizations->getInt();

  //
  // now do python call
  //

  QString appDir = SimCenterPreferences::getInstance()->getAppDir();
  QString IstanbulScript = appDir + QDir::separator() + "applications" + QDir::separator()
    + "createEVENT" + QDir::separator() + "Istanbul" + QDir::separator() + "IstanbulRun.py";
  
  QStringList args; args << QString("--lat") << QString::number(latitude)
                         << QString("--lng") << QString::number(longitude)
                         << QString("-g") << currentGrid
                         << QString("-n") << QString::number(numMotion)
                         << QString("-o") << destDir
                         << "--username" << username
                         << "--password" << password;
  
  //
  // run the download in a Thread using RunPythonInThread
  //    NOTE: do not invoke destructor .. class kills itself when python app finishes
  //
  
  errorMessage("Istanbul: Getting Motions from Designsafe repository. This may take a few minutes");
  getMotions->setEnabled(false);
  downloadedMotions = false;
  motionsDownloading = true;
  RunPythonInThread *thePythonProcess = new RunPythonInThread(IstanbulScript, args, tmpLocation->getDirName());
  connect(thePythonProcess, &RunPythonInThread::processFinished, this, &IstanbulSingleSite::motionsDownloaded);
  thePythonProcess->runProcess();
}

void
IstanbulSingleSite::motionsDownloaded(int exitCode) {
  motionsDownloading = false;
  if (exitCode == 0)
    downloadedMotions = true;
  getMotions->setEnabled(true);
  errorMessage("Istanbul motions download completed");
}

bool
IstanbulSingleSite::outputAppDataToJSON(QJsonObject &jsonObject)
{
  qDebug() << "IstanbulSingleSite::outputAppDataToJSON() - should not be called";
  Q_UNUSED(jsonObject);
  return false;
}

bool
IstanbulSingleSite::inputAppDataFromJSON(QJsonObject &jsonObject)
{
  qDebug() << "IstanbulSingleSite::inputAppDataFromJSON() - should not be called";
  Q_UNUSED(jsonObject);
  return false;  
}

bool
IstanbulSingleSite::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "PhysicsBasedMotion"; // for backend processing
    jsonObject["sub_type"] = "Istanbul";    
    QString downloadedDir = tmpLocation->getDirName();
    QDir downloadDirectory(downloadedDir);
    QStringList motions = downloadDirectory.entryList(QStringList() << "*.json",QDir::Files);
    QJsonArray motionList;
    for (const QString& motion : motions) {
      motionList.append(motion);
    }
    jsonObject["motions"]=motionList;
    jsonObject["fileType"]="JSON";
    lat->outputToJSON(jsonObject);
    lng->outputToJSON(jsonObject);
    numRealizations->outputToJSON(jsonObject);
    gridType->outputToJSON(jsonObject);
    tmpLocation->outputToJSON(jsonObject);
    
    return true;
}

bool
IstanbulSingleSite::inputFromJSON(QJsonObject &jsonObject)
{
    lat->inputFromJSON(jsonObject);
    lng->inputFromJSON(jsonObject);
    numRealizations->inputFromJSON(jsonObject);
    gridType->inputFromJSON(jsonObject);
    tmpLocation->inputFromJSON(jsonObject);
    
    return true;
}


bool
IstanbulSingleSite::copyFiles(QString &destDir)
{
  //
  // check we are not still downloading
  //
  
  if (motionsDownloading == true) {
    errorMessage(QString("Istanbul: Motions Still Downloading"));
    return false;
  }
  
  QString downloadedDir = tmpLocation->getDirName();
  QDir downloadDirectory(downloadedDir);
  QStringList motions = downloadDirectory.entryList(QStringList() << "*.json",QDir::Files);

  //
  // first check we have actual files to copy
  //
  
  if (downloadedMotions == false && motions.count() == 0) {
    
    errorMessage(QString("Istanbul: No motions Exist"));
    statusMessage(QString("Return to the EVT panel and press 'Get Motions' to start the download process. Then wait till downloaded appears in the program output"));    
    return false;
  }

  else if (downloadedMotions == false) {
    
    statusMessage(QString("Istanbul: No New Downloaded Motions"));      
    switch( QMessageBox::question( 
                  this, 
                  tr("Istanbul"), 
                  tr("Istanbul has detected that motions exist in the tmp folder, but these are old from a previous download selection. Do you wish to continue?"),
                  QMessageBox::Yes | 
                  QMessageBox::No,
                  QMessageBox::Yes ) )
      {
      case QMessageBox::Yes:
        break;
      case QMessageBox::No:
        return false;
        break;
      default:
        
        break;
      }
  }

  //
  // now copy files to input_data instead of dest_dir
  //


  // mkdir input_data, same level as dest_dir
  
  QDir destinationFolder(destDir);
  destinationFolder.cdUp();
  QString inputDataDirPath = destinationFolder.absoluteFilePath("input_data");
    
  if (destinationFolder.mkpath(inputDataDirPath) == false) {
    this->errorMessage("Istanbul failed to create folder: ");
    this->errorMessage(inputDataDirPath);
    return false;
  }

  // copyfiles
  
  SimCenterAppWidget::copyPath(downloadedDir, inputDataDirPath, false);
  return true;
}

void
IstanbulSingleSite::clear(void)
{

}

bool
IstanbulSingleSite::outputCitation(QJsonObject &jsonObject)
{
  jsonObject.insert("citation", QString("Zhang, W., J. Crempien, K. Zhong, P. Chen, P. Arduino, E. Taciroglu. (2023) A suite of 57 broadband physics-based ground motion simulations for the Istanbul region, in Regional-scale physics-based ground motion simulation for Istanbul, Turkey. DesignSafe-CI. https://doi.org/10.17603/ds2-e7nq-8d52 v1"));
  jsonObject.insert("description", QString("A suite of 57 broadband physics-based ground motion simulations for the Istanbul region"));
  return true;
}

