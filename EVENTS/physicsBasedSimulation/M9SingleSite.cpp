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

// Written: fmckenna, Amin Pakzad
#include "MainWindow.h"
#include "M9SingleSite.h"
#include <SC_DoubleLineEdit.h>
#include <SC_IntLineEdit.h>
#include <SC_DirEdit.h>
#include <SC_ComboBox.h>
#include <SC_CheckBox.h>
#include <QApplication>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QWebEngineView>
#include <QWebEngineSettings>

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

M9SingleSite::M9SingleSite(QWidget *parent)
:SimCenterAppWidget(parent), count(0), downloadedMotions(false), motionsDownloading(false)
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
  
  theLayout->addWidget(new QLabel("Grid"),0,0);
  QStringList listGrids; listGrids  << "All" << "A" << "B" << "C" << "D";
  gridType = new SC_ComboBox("gridType",listGrids);
  theLayout->addWidget(gridType, 0, 1);
  
  theLayout->addWidget(new QLabel("num Motions"),0,2);
  numRealizations = new SC_IntLineEdit("numRealizations",10);
  theLayout->addWidget(numRealizations,0,3);
  
  theLayout->addWidget(new QLabel("latitude"),2,0);
  lat = new SC_DoubleLineEdit("latitude",47.620422);
  theLayout->addWidget(lat,2,1);
  theLayout->addWidget(new QLabel("longitude"),2,2);
  lng = new SC_DoubleLineEdit("longitude",-122.349358);
  theLayout->addWidget(lng,2,3);

  QString dirPath = SimCenterPreferences::getInstance()->getLocalWorkDir() + QDir::separator() + "M9";
  theLayout->addWidget(new QLabel("tmp Directory"),3,0);  
  tmpLocation = new SC_DirEdit("tmpLocation");
  tmpLocation->setDirName(dirPath);
  theLayout->addWidget(tmpLocation,3,1,1,3);

  // Add a checkbox to allow the user to select if they want to use API or not
  theLayout->addWidget(new QLabel("API"), 4, 0);            // Column 4
  useAPI = new SC_CheckBox("useAPI", false);
  theLayout->addWidget(useAPI, 4, 1); 
  // add the explanation for the API 
  QLabel *apiExplanation = new QLabel("Check this box if you want to use the API to get the motions. If you do not check this box, the motions will be Extracted from Designsafe repository.");
  apiExplanation->setWordWrap(true);
  theLayout->addWidget(apiExplanation, 4, 2, 1, 2);


  getMotions = new QPushButton("Get Motions");
  theLayout->addWidget(getMotions, 5,1,1,3);
  connect(getMotions, &QPushButton::clicked, this, [=](){
    this->downloadMotions();
  });

  QLabel *citation = new QLabel("Frankel, A., Wirth, E., Marafi, N., Vidale, J., and Stephenson, W. (2018), Broadband Synthetic Seismograms for Magnitude 9 Earthquakes on the Cascadia Megathrust Based on 3D Simulations and Stochastic Synthetics, Part 1: Methodology and Overall Results. Bulletin of the Seismological Society of America, 108 (5A), 2347–2369. doi: https://doi.org/10.1785/0120180034");
  citation->setWordWrap(true);
  
  theLayout->addWidget(citation, 6,0,1,4);  
  
  theLayout->setRowStretch(7,1);
  theLayout->setColumnStretch(1,1);
  theLayout->setColumnStretch(3,1); 
  theLayout->setColumnStretch(4,1);

  leftLayout->addWidget(inputParameters,0,0);  


  QGroupBox *mapboxM9 = new QGroupBox("Grid Location");

  // ===========================================================
  // creating right widget
  // ===========================================================
  QWidget *rightWidget = new QWidget();

  QGridLayout *rightLayout = new QGridLayout(rightWidget);

  QGroupBox *mapbox = new QGroupBox("Grid Maps");
  QGridLayout *maplayout = new QGridLayout(mapbox);

  //  QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
  //   QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
  

  // webView1->page()->setDevToolsPage(new QWebEnginePage());
  webView1 = new QWebEngineView();
  webView1->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
  webView1->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);  

  
  QString appDir = SimCenterPreferences::getInstance()->getAppDir();
  QString mappscript = appDir + QDir::separator() + "applications" + QDir::separator()
    + "createEVENT" + QDir::separator() + "M9" + QDir::separator();
  // webView1->setUrl(QUrl("https://www.google.com/maps"));
  webView1->load(QUrl::fromLocalFile(mappscript+"All_grid.html"));
  webView1->show();
  maplayout->addWidget(webView1,0,0);
  rightLayout->addWidget(mapbox,0,0);



  leftWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  rightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mainLayout->addWidget(leftWidget);
  mainLayout->addWidget(rightWidget);







  // create a signal if gridType changed to change the map
  connect(gridType, &QComboBox::currentTextChanged, this, [=](){
    QString currentGrid=gridType->currentText();
    webView1->load(QUrl::fromLocalFile(mappscript+currentGrid+"_grid.html"));
  });
}

M9SingleSite::~M9SingleSite()
{

  
  
  
}

void
M9SingleSite::downloadMotions(void)
{
  qDebug() << "M9SingleSite::downloadMotions called";
  
  // if user has not checked 
  if (!useAPI->isChecked()) {
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
    // make the dialog appear in the center of the screen i nstead of the top left corner
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
    errorMessage(QString("a BUG in M9SingleSite as this SHOULD NOT HAPPEN .. NO directory: ") + destDir);
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
  QString m9Script = appDir + QDir::separator() + "applications" + QDir::separator()
    + "createEVENT" + QDir::separator() + "M9" + QDir::separator() + "M9Run.py";
  
  QStringList args; args << QString("--lat") << QString::number(latitude)
			 << QString("--lng") << QString::number(longitude)
			 << QString("-g") << currentGrid
			 << QString("-n") << QString::number(numMotion)
			 << QString("-o") << destDir
       << QString("--API") << QString::number(useAPI->isChecked())
       << "--username" << username
       << "--password" << password;
  
  /*
  QJsonObject information;
  information["LocationFlag"]=true;
  information["APIFLAG"]=true;
  information["latitude"]=latitude;
  information["longitude"]=longitude;
  information["grid_type"]=currentGrid;
  information["number_of_realizations"]=numMotion;
  information["directory"]=destDir;

  QJsonDocument jsonDoc(information);

  // Convert QJsonDocument to JSON string
  QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
  
  // Write JSON data to a file
  QString informationFile = destDir + QDir::separator() + "information.json";
  QFile outputFile(informationFile);
  if(outputFile.open(QIODevice::WriteOnly)) {
      outputFile.write(jsonData);
      outputFile.close();
  } else {
    QString msg("M9SingleSite::Could not open file" + informationFile);
    errorMessage(msg);
    return;
  }
  QStringList args; args << informationFile;
  */
  
  // ModularPython *thePythonApp = new ModularPython(tmpLocation->getDirName());
  // errorMessage("M9 Downloading Motions .. this takes about 3 minutes per motion!");
  // thePythonApp->run(m9Script,args);
  // errorMessage("M9 Motions Downloaded");
  // delete thePythonApp;  

  //
  // run the download in a Thread using RunPythinInThread
  //    NOTE: do not  invoke destructor .. class kills itself when python app finishes
  //
  // I useAPI is checked, then use the API print the message to the user
  if (useAPI->isChecked()) {
    errorMessage("M9 Downloading Motions using API .. this takes about 3 minutes per motion");

  } else {
    errorMessage("M9 running a app in Designsafe to download motions");
  }
  getMotions->setEnabled(false);
  downloadedMotions = false;
  motionsDownloading = true;
  RunPythonInThread *thePythonProcess = new RunPythonInThread(m9Script, args, tmpLocation->getDirName());
  connect(thePythonProcess, &RunPythonInThread::processFinished, this, &M9SingleSite::motionsDownloaded);
  thePythonProcess->runProcess();
}

void
M9SingleSite::motionsDownloaded(int exitCode) {
  motionsDownloading = false;
  if (exitCode == 0)
    downloadedMotions = true;
  getMotions->setEnabled(true);
}

bool
M9SingleSite::outputAppDataToJSON(QJsonObject &jsonObject)
{
  qDebug() << "M9SingleSite::outputAppDataToJSON() - should not be called";
  Q_UNUSED(jsonObject);
  return false;
}

bool
M9SingleSite::inputAppDataFromJSON(QJsonObject &jsonObject)
{
  qDebug() << "M9SingleSite::inputAppDataFromJSON() - should not be called";
  Q_UNUSED(jsonObject);
  return false;  
}

bool
M9SingleSite::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "PhysicsBasedMotion"; // for backend processing
    jsonObject["sub_type"] = "M9";    
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
    useAPI->outputToJSON(jsonObject);
    
    return true;
}

bool
M9SingleSite::inputFromJSON(QJsonObject &jsonObject)
{
    lat->inputFromJSON(jsonObject);
    lng->inputFromJSON(jsonObject);
    numRealizations->inputFromJSON(jsonObject);
    gridType->inputFromJSON(jsonObject);
    tmpLocation->inputFromJSON(jsonObject);
    useAPI->inputFromJSON(jsonObject);
    
    return true;
}


bool
M9SingleSite::copyFiles(QString &destDir)
{

  //
  // check we are not still downloading
  //
  
  if (motionsDownloading == true) {
    errorMessage(QString("M9: Motions Still Downloading"));
    return false;
  }
  
  QString downloadedDir = tmpLocation->getDirName();
  QDir downloadDirectory(downloadedDir);
  QStringList motions = downloadDirectory.entryList(QStringList() << "*.json",QDir::Files);

  //
  // ccheck we have actual files to copy
  //
  
  if (downloadedMotions == false && motions.count() == 0) {
    
    errorMessage(QString("M9: No motions Exist"));
    statusMessage(QString("Return to the EVT panel and press 'Download Records' to start the download process. Then wait till downloaded appears in the program output"));    
    return false;
  }

  else if (downloadedMotions == false) {
    
    statusMessage(QString("M9: No New Downloaded Motions"));      
    switch( QMessageBox::question( 
				  this, 
				  tr("M9"), 
				  tr("M9 has detected that motions exist in the tmp folder, but these are old from a previous download selection. Do you wish to continue?"),
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
    this->errorMessage("M9 failed to create folder: ");
    this->errorMessage(inputDataDirPath);
    return false;
  }

  // copyfiles
  
  SimCenterAppWidget::copyPath(downloadedDir, inputDataDirPath, false);
  return true;
}

void
M9SingleSite::clear(void)
{

}

bool
M9SingleSite::outputCitation(QJsonObject &jsonObject)
{
  jsonObject.insert("citation",QString("Frankel, A., Wirth, E., Marafi, N., Vidale, J., and Stephenson, W. (2018), Broadband Synthetic Seismograms for Magnitude 9 Earthquakes on the Cascadia Megathrust Based on 3D Simulations and Stochastic Synthetics, Part 1: Methodology and Overall Results. Bulletin of the Seismological Society of America, 108 (5A), 2347–2369. doi: https://doi.org/10.1785/0120180034"));
  jsonObject.insert("description",QString("The ground motions used in the simulations were created as part of the M9 project led by the University of Washington. The M9 project generated a number of motions to study the  potential impacts of a magnitude 9 (M9) earthquake on the Cascadia Subduction Zone, which is located off the coast of the Pacific Northwest region of the United States"));
  
  return true;
}

