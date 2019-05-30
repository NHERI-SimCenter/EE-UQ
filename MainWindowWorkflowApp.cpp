
// Written: fmckenna
// Purpose: to test the INputWidgetSheetBM widget

#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>
#include "MainWindowWorkflowApp.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopServices>
#include <sectiontitle.h>

//#include <InputWidgetEE_UQ.h>
#include <WorkflowAppWidget.h>

#include "SimCenterTableWidget.h"
#include <QDesktopWidget>
#include <HeaderWidget.h>
#include <FooterWidget.h>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include <RemoteService.h>


MainWindowWorkflowApp::MainWindowWorkflowApp(QString appName, WorkflowAppWidget *theApp, RemoteService *theService, QWidget *parent)
  : QMainWindow(parent), theRemoteInterface(theService), inputWidget(theApp), loggedIn(false), isAutoLogin(false)
{
    //
    // create a layout & widget for central area of this QMainWidget
    //  to this widget we will add a header, selection, button and footer widgets
    //

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    centralWidget->setLayout(layout);

    //
    // resize to primary screen
    //
      
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int height = this->height()<int(0.85*rec.height())?int(0.85*rec.height()):this->height();
    int width  = this->width()<int(0.85*rec.width())?int(0.85*rec.width()):this->width();
    this->resize(width, height);

    //
    // add SimCenter Header
    //

    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(appName);
    layout->addWidget(header);

    // place a location for messages;
    QHBoxLayout *layoutMessages = new QHBoxLayout();
    errorLabel = new QLabel();
    layoutMessages->addWidget(errorLabel);
    header->appendLayout(layoutMessages);

    // place login info
    QHBoxLayout *layoutLogin = new QHBoxLayout();
    QLabel *name = new QLabel();
    //name->setText("");
    loginButton = new QPushButton();
    loginButton->setText("Login");
    layoutLogin->addWidget(name);
    layoutLogin->addWidget(loginButton);
    layoutLogin->setAlignment(Qt::AlignLeft);
    header->appendLayout(layoutLogin);

    layout->addWidget(inputWidget);

    // layout->addStretch();

    //
    // add run, run-DesignSafe and exit buttons into a new widget for buttons
    //

    // create the buttons widget and a layout for it
    QHBoxLayout *pushButtonLayout = new QHBoxLayout();
    QWidget *buttonWidget = new QWidget();
    buttonWidget->setLayout(pushButtonLayout);

    // create a bunch of buttons

    QPushButton *runButton = new QPushButton();
    runButton->setText(tr("RUN"));
    pushButtonLayout->addWidget(runButton);

    QPushButton *runDesignSafeButton = new QPushButton();
    runDesignSafeButton->setText(tr("RUN at DesignSafe"));
    pushButtonLayout->addWidget(runDesignSafeButton);

    QPushButton *getDesignSafeButton = new QPushButton();
    getDesignSafeButton->setText(tr("GET from DesignSafe"));
    pushButtonLayout->addWidget(getDesignSafeButton);

    QPushButton *exitButton = new QPushButton();
    exitButton->setText(tr("Exit"));
    pushButtonLayout->addWidget(exitButton);

    //
    // create login window for when loogged in clicked
    //

    loginWindow = new QWidget();
    loginWindow->setWindowTitle("Login to DesignSafe");
    QGridLayout *loginLayout = new QGridLayout();
    SectionTitle *info=new SectionTitle();
    info->setText(tr("DesignSafe User Account Info:"));

    QLabel *nameLabel = new QLabel();
    nameLabel->setText("Username:");
    QLabel *passwordLabel = new QLabel();
    passwordLabel->setText("Password:");
    nameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginSubmitButton = new QPushButton();
    loginSubmitButton->setText("Login");
    loginLayout->addWidget(info,0,0,2,2,Qt::AlignBottom);
    loginLayout->addWidget(nameLabel,2,0);
    loginLayout->addWidget(nameLineEdit,2,1);
    loginLayout->addWidget(passwordLabel,3,0);
    loginLayout->addWidget(passwordLineEdit,3,1);
    loginLayout->addWidget(loginSubmitButton,4,2);
    loginWindow->setLayout(loginLayout);

    /*
    loginWindow->setStyleSheet("QComboBox {background: #FFFFFF;} \
  QGroupBox {font-weight: bold;}\
  QLineEdit {background-color: #FFFFFF; border: 2px solid darkgray;} \
  QTabWidget::pane {background-color: #ECECEC; border: 1px solid rgb(239, 239, 239);}");
  */

    //
    // connect some signals and slots
    //

    // login
    connect(loginButton,&QPushButton::clicked,this,[this](bool)
    {
        isAutoLogin = false;
        onLoginButtonClicked();
    });
    connect(loginSubmitButton,SIGNAL(clicked(bool)),this,SLOT(onLoginSubmitButtonClicked()));
    connect(this,SIGNAL(attemptLogin(QString, QString)),theRemoteInterface,SLOT(loginCall(QString, QString)));
    connect(theRemoteInterface,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));
    connect(passwordLineEdit, &QLineEdit::returnPressed, this, [this](){
        this->onLoginSubmitButtonClicked();
    });
    // logout
    connect(this,SIGNAL(logout()),theRemoteInterface,SLOT(logoutCall()));
    connect(theRemoteInterface,SIGNAL(logoutReturn(bool)),this,SLOT(logoutReturn(bool)));

    // allow remote interface to send error and status messages
    connect(theRemoteInterface,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(statusMessage(QString)),this,SLOT(statusMessage(QString)));

    connect(inputWidget,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(inputWidget,SIGNAL(sendStatusMessage(QString)),this,SLOT(statusMessage(QString)));
    connect(inputWidget,SIGNAL(sendFatalMessage(QString)),this,SLOT(fatalMessage(QString)));


    // connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    // connect job manager
    connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    connect(runDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteRunButtonClicked()));
    connect(getDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteGetButtonClicked()));
    connect(exitButton, SIGNAL(clicked(bool)),this,SLOT(onExitButtonClicked()));

    /*
   connect(uq,SIGNAL(uqWidgetChanged()), this,SLOT(onDakotaMethodChanged()));

   connect(fem,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
   connect(random,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
   connect(results,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
   connect(uq,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
   */

    // add button widget to layout
    layout->addWidget(buttonWidget);

    //
    // add SimCenter footer
    //

    FooterWidget *footer = new FooterWidget();
    layout->addWidget(footer);

    this->setCentralWidget(centralWidget);

    this->createActions();

    inputWidget->setMainWindow(this);

    //
    // strings needed in about menu, use set methods to override
    //
    manualURL = QString("");
    feedbackURL = QString("https://docs.google.com/forms/d/e/1FAIpQLSfh20kBxDmvmHgz9uFwhkospGLCeazZzL770A2GuYZ2KgBZBA/viewform");
    featureRequestURL = QString("https://docs.google.com/forms/d/e/1FAIpQLScTLkSwDjPNzH8wx8KxkyhoIT7AI9KZ16Wg9TuW1GOhSYFOag/viewform");
    versionText = QString("");
    citeText = QString("");
    aboutText = QString(tr("This is a SeimCenter Workflow Applicatios"));
    copyrightText = QString("\
                            <p>\
                            The source code is licensed under a BSD 2-Clause License:<p>\
                            \"Copyright (c) 2017-2018, The Regents of the University of California (Regents).\"\
                            All rights reserved.<p>\
                            <p>\
                            Redistribution and use in source and binary forms, with or without \
                            modification, are permitted provided that the following conditions are met:\
                            <p>\
                            1. Redistributions of source code must retain the above copyright notice, this\
                            list of conditions and the following disclaimer.\
                            \
                            \
                            2. Redistributions in binary form must reproduce the above copyright notice,\
                            this list of conditions and the following disclaimer in the documentation\
                            and/or other materials provided with the distribution.\
                            <p>\
                            THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\
                            ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\
                            WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\
                            DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\
                            ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\
                            (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\
                            LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\
            ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\
            (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\
            SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\
            <p>\
            The views and conclusions contained in the software and documentation are those\
            of the authors and should not be interpreted as representing official policies,\
            either expressed or implied, of the FreeBSD Project.\
            <p>\
            REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, \
            THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\
            THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS \
            PROVIDED \"AS IS\". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,\
            UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            The compiled binary form of this application is licensed under a GPL Version 3 license.\
            The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
            included in the packaging of this application. \
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            This software makes use of the QT packages (unmodified): core, gui, widgets and network\
                                                                     <p>\
                                                                     QT is copyright \"The Qt Company Ltd&quot; and licensed under the GNU Lesser General \
                                                                     Public License (version 3) which references the GNU General Public License (version 3)\
      <p>\
      The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
      included in the packaging of this application. \
      <p>\
      ");
}

MainWindowWorkflowApp::~MainWindowWorkflowApp()
{

}


bool MainWindowWorkflowApp::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool MainWindowWorkflowApp::saveAs()
{
    //
    // get filename
    //

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;

    // and save the file
    return saveFile(dialog.selectedFiles().first());
}

void MainWindowWorkflowApp::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}


void MainWindowWorkflowApp::openFile(QString fileName)
{
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindowWorkflowApp::newFile()
{
    // clear old
    inputWidget->clear();

    // set currentFile blank
    setCurrentFile(QString());
}


void MainWindowWorkflowApp::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    //  setWindowModified(false);

    QString shownName = currentFile;
    if (currentFile.isEmpty())
        shownName = "untitled.json";

    setWindowFilePath(shownName);
}

bool MainWindowWorkflowApp::saveFile(const QString &fileName)
{
    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }


    //
    // create a json object, fill it in & then use a QJsonDocument
    // to write the contents of the object to the file in JSON format
    //

    QJsonObject json;
    inputWidget->outputToJSON(json);
    QJsonDocument doc(json);
    file.write(doc.toJson());

    // close file
    file.close();

    // set current file
    setCurrentFile(fileName);

    return true;
}

void MainWindowWorkflowApp::loadFile(const QString &fileName)
{
    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // validate the document
    // JsonValidator class already takes a model type param, add additional model types as required

    /*
    JsonValidator *jval = new JsonValidator();
    jval->validate(this, BIM, fileName);
*/

    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();

    // given the json object, create the C++ objects
    if ( ! (currentFile.isNull() || currentFile.isEmpty()) ) {
        inputWidget->clear();
    }
    inputWidget->inputFromJSON(jsonObj);

    setCurrentFile(fileName);
}


void MainWindowWorkflowApp::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));


    //const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    //const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));

    //QToolBar *fileToolBar = addToolBar(tr("File"));

    //    QAction *newAction = new QAction(tr("&New"), this);
    //    newAction->setShortcuts(QKeySequence::New);
    //    newAction->setStatusTip(tr("Create a new file"));
    //    connect(newAction, &QAction::triggered, this, &MainWindowWorkflowApp::newFile);
    //    fileMenu->addAction(newAction);

    QAction *openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindowWorkflowApp::open);
    fileMenu->addAction(openAction);
    //fileToolBar->addAction(openAction);


    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the document to disk"));
    connect(saveAction, &QAction::triggered, this, &MainWindowWorkflowApp::save);
    fileMenu->addAction(saveAction);

    QAction *saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setStatusTip(tr("Save the document with new filename to disk"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindowWorkflowApp::saveAs);
    fileMenu->addAction(saveAsAction);

    // strangely, this does not appear in menu (at least on a mac)!! ..
    // does Qt not allow as in tool menu by default?
    // check for yourself by changing Quit to drivel and it works
    QAction *exitAction = new QAction(tr("&Quit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    exitAction->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&Version"), this, &MainWindowWorkflowApp::version);
    QAction *infoAct = helpMenu->addAction(tr("&About"), this, &MainWindowWorkflowApp::about);
    QAction *manualAct = helpMenu->addAction(tr("&Manual"), this, &MainWindowWorkflowApp::manual);
    QAction *submitAct = helpMenu->addAction(tr("&Provide Feedback"), this, &MainWindowWorkflowApp::submitFeedback);
    QAction *submitFeature = helpMenu->addAction(tr("&Submit Feature Request"), this, &MainWindowWorkflowApp::submitFeatureRequest);
    QAction *citeAct = helpMenu->addAction(tr("&How to Cite"), this, &MainWindowWorkflowApp::cite);
    QAction *copyrightAct = helpMenu->addAction(tr("&License"), this, &MainWindowWorkflowApp::copyright);
}




void MainWindowWorkflowApp::onLoginButtonClicked() {

    if (loggedIn == false) {
        numTries = 0;
        loginWindow->show();
    } else {
        loggedIn = false;
        emit logout();
    }
}

void MainWindowWorkflowApp::onLoginSubmitButtonClicked() {

    int maxNumTries = 3;

    if (loggedIn == false && numTries < maxNumTries) {
        // obtain login info
        QString login = nameLineEdit->text();
        QString password = passwordLineEdit->text();
        if (login.size() == 0) {
            login = "no_username";
        }
        if (password.size() == 0)
            password = "no_password";

        emit attemptLogin(login, password);
        return;
    }
}


void
MainWindowWorkflowApp::attemptLoginReturn(bool ok){

    int maxNumTries = 3;

    if (ok == true) {
        //      emit updateJobTable("");
        loginWindow->hide();
        loggedIn = true;
        loginButton->setText("Logout");
        //this->enableButtons();

        //theJobManager->up
        if(isAutoLogin)
        {
            onRemoteRunButtonClicked();
            isAutoLogin = false;
        }
    } else {
        loggedIn = false;

        numTries++;

        if (numTries >= maxNumTries) {
            loginWindow->hide();
            nameLineEdit->setText("");
            passwordLineEdit->setText("");
            this->errorMessage("ERROR: Max Login Attempts Exceeded .. Contact DesignSafe for password help");
        }
    }
}


void
MainWindowWorkflowApp::logoutReturn(bool ok){

    if (ok == true) {
        loggedIn = false;
        loginButton->setText("Login");
        //this->disableButtons();
        // bring up login button
        //  this->onLoginButtonClicked();
    }
}

void
MainWindowWorkflowApp::onRunButtonClicked() {
    inputWidget->onRunButtonClicked();
}

void
MainWindowWorkflowApp::onRemoteRunButtonClicked(){
    if (loggedIn == true)
        inputWidget->onRemoteRunButtonClicked();
    else
    {
        this->errorMessage(tr("You must log in to DesignSafe before you can run a remote job"));
        this->onLoginButtonClicked();
        isAutoLogin = true;
    }
}

void
MainWindowWorkflowApp::onRemoteGetButtonClicked(){
    if (loggedIn == true)
        inputWidget->onRemoteGetButtonClicked();
    else
        this->errorMessage(tr("You Must LOGIN (button top right) before you can run retrieve remote data"));
};

void MainWindowWorkflowApp::onExitButtonClicked(){
    //RandomVariablesContainer *theParameters = uq->getParameters();
    inputWidget->onExitButtonClicked();
    QCoreApplication::exit(0);
}


void
MainWindowWorkflowApp::statusMessage(const QString msg){
    errorLabel->setText(msg);
    qDebug() << "STATUS MESSAGE" << msg;
    QApplication::processEvents();
}

void
MainWindowWorkflowApp::errorMessage(const QString msg){
    errorLabel->setText(msg);
    qDebug() << "ERROR MESSAGE" << msg;
    QApplication::processEvents();
}

void
MainWindowWorkflowApp::fatalMessage(const QString msg){
    errorLabel->setText(msg);
    qDebug() << "FATAL MESSAGE" << msg;
}


void MainWindowWorkflowApp::version()
{
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(versionText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}

void MainWindowWorkflowApp::cite()
{
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(citeText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}

void MainWindowWorkflowApp::about()
{
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(aboutText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}

void MainWindowWorkflowApp::submitFeedback()
{
    QDesktopServices::openUrl(QUrl(feedbackURL, QUrl::TolerantMode));
    //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}

void MainWindowWorkflowApp::manual()
{
    QDesktopServices::openUrl(QUrl(manualURL, QUrl::TolerantMode));
    //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}

void MainWindowWorkflowApp::submitFeatureRequest()
{
    QDesktopServices::openUrl(QUrl(featureRequestURL, QUrl::TolerantMode));
    //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}

void MainWindowWorkflowApp::copyright()
{
  QMessageBox msgBox;
  QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  msgBox.setText(copyrightText);
  QGridLayout *layout = (QGridLayout*)msgBox.layout();
  layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
  msgBox.exec();

}

void 
MainWindowWorkflowApp::setCopyright(QString &newText)
{
  copyrightText = newText;
}

void 
MainWindowWorkflowApp::setVersion(QString &newText)
{
  versionText = newText;
}

void 
MainWindowWorkflowApp::setAbout(QString &newText)
{
  aboutText = newText;
}

void
MainWindowWorkflowApp::setDocumentationURL(QString &newText)
{
  manualURL = newText;
}

void
MainWindowWorkflowApp::setFeedbackURL(QString &newText)
{
  feedbackURL = newText;
}

void
MainWindowWorkflowApp::setFeatureURL(QString &newText)
{
  featureRequestURL = newText;
}

void
MainWindowWorkflowApp::setCite(QString &newText)
{
  citeText = newText;
}


