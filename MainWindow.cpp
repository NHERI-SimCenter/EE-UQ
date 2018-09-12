
// Written: fmckenna
// Purpose: to test the INputWidgetSheetBM widget

#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>
#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMenuBar>
#include <QAction>
#include <QMenu>
#include <QApplication>

#include <InputWidgetEE_UQ.h>
#include "SimCenterTableWidget.h"
#include <QDesktopWidget>
#include <HeaderWidget.h>
#include <FooterWidget.h>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include <AgaveCurl.h>


MainWindow::MainWindow(AgaveCurl *theInterface, QWidget *parent)
  : QMainWindow(parent), theRemoteInterface(theInterface)
{

    //
    // create a layout & widget for central area of this QMainWidget
    //  to this widget we will add a header, selection, button and footer widgets
    //

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    centralWidget->setLayout(layout);

    QRect rec = QApplication::desktop()->screenGeometry();

    int height = 0.7*rec.height();
    int width = 0.7*rec.width();

    this->resize(width, height);

    //
    // add SimCenter Header
    //

    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(tr("EE-UQ: Response of Building to Earthquake"));
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


    inputWidget = new InputWidgetEE_UQ();
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
    QGridLayout *loginLayout = new QGridLayout();
    QLabel *nameLabel = new QLabel();
    nameLabel->setText("username:");
    QLabel *passwordLabel = new QLabel();
    passwordLabel->setText("password:");
    nameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginSubmitButton = new QPushButton();
    loginSubmitButton->setText("Login");
    loginLayout->addWidget(nameLabel,0,0);
    loginLayout->addWidget(nameLineEdit,0,1);
    loginLayout->addWidget(passwordLabel,1,0);
    loginLayout->addWidget(passwordLineEdit,1,1);
    loginLayout->addWidget(loginSubmitButton,2,2);
    loginWindow->setLayout(loginLayout);

    //
    // connect some signals and slots
    //

    // login
    connect(loginButton,SIGNAL(clicked(bool)),this,SLOT(onLoginButtonClicked()));
    connect(loginSubmitButton,SIGNAL(clicked(bool)),this,SLOT(onLoginSubmitButtonClicked()));
    connect(this,SIGNAL(attemptLogin(QString, QString)),theRemoteInterface,SLOT(loginCall(QString, QString)));
    connect(theRemoteInterface,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));

    // logout
    connect(this,SIGNAL(logout()),theRemoteInterface,SLOT(logoutCall()));
    connect(theRemoteInterface,SIGNAL(logoutReturn(bool)),this,SLOT(logoutReturn(bool)));

    // allow remote interface to send error and status messages
    connect(theRemoteInterface,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(statusMessage(QString)),this,SLOT(statusMessage(QString)));

    // connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    // connect job manager
    connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    connect(runDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteRunButtonClicked()));
    connect(getDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteButtonClicked()));
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


}

MainWindow::~MainWindow()
{

}


bool MainWindow::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool MainWindow::saveAs()
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

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::newFile()
{
    // clear old
    inputWidget->clear();

    // set currentFile blank
    setCurrentFile(QString());
}


void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    //  setWindowModified(false);

    QString shownName = currentFile;
    if (currentFile.isEmpty())
        shownName = "untitled.json";

    setWindowFilePath(shownName);
}

bool MainWindow::saveFile(const QString &fileName)
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

void MainWindow::loadFile(const QString &fileName)
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


void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));


    //const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    //const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));

    //QToolBar *fileToolBar = addToolBar(tr("File"));

    QAction *newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new file"));
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAction);
    //fileToolBar->addAction(newAction);

    QAction *openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAction);
    //fileToolBar->addAction(openAction);


    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the document to disk"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAction);

    QAction *saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setStatusTip(tr("Save the document with new filename to disk"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAction);

    // strangely, this does not appear in menu (at least on a mac)!! ..
    // does Qt not allow as in tool menu by default?
    // check for yourself by changing Quit to drivel and it works
    QAction *exitAction = new QAction(tr("&Quit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    // exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAction);

    // the Model Menu cotains is a check list of which model type is currently being edited
    // to get the current value check each menu item
    // only one menu item should be selected at a time, the dafault on startup is BIM
    QMenu *modelMenu = menuBar()->addMenu(tr("&Model"));

    QAction *bimAction = new QAction(tr("&BIM"), this);
    bimAction->setCheckable(true);
    bimAction->setChecked(true);
    bimAction->setStatusTip(tr("Create a Building Information Model"));
    modelMenu->addAction(bimAction);
    //todo: the menu should connect to a funcation that presents a confirmation dialog, saves the current model to file,
    // unchecks the previous value, reloads thhe main withdow with the appropriate InputWIdetSheetXX class

    QAction *samAction = new QAction(tr("&SAM"), this);
    samAction->setCheckable(true);
    samAction->setStatusTip(tr("Create a Structure Information Model"));
    modelMenu->addAction(samAction);
    //todo: the menu should connect to a funcation that presents a confirmation dialog, saves the current model to file,
    // unchecks the previous value, reloads thhe main withdow with the appropriate InputWIdetSheetXX class

    /*
 cutAction = new QAction(tr("Cu&t"), this);
 cutAction->setIcon(QIcon(":/images/cut.png"));
 cutAction->setShortcut(QKeySequence::Cut);
 cutAction->setStatusTip(tr("Cut the current selection's contents "
                            "to the clipboard"));
 //connect(cutAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(cut()));

 copyAction = new QAction(tr("&Copy"), this);
 copyAction->setIcon(QIcon(":/images/copy.png"));
 copyAction->setShortcut(QKeySequence::Copy);
 copyAction->setStatusTip(tr("Copy the current selection's contents "
                             "to the clipboard"));
 //connect(copyAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(copy()));

 pasteAction = new QAction(tr("&Paste"), this);
 pasteAction->setIcon(QIcon(":/images/paste.png"));
 pasteAction->setShortcut(QKeySequence::Paste);
 pasteAction->setStatusTip(tr("Paste the clipboard's contents into "
                              "the current selection"));
 //connect(pasteAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(paste()));

 deleteAction = new QAction(tr("&Delete"), this);
 deleteAction->setShortcut(QKeySequence::Delete);
 deleteAction->setStatusTip(tr("Delete the current selection's "
                               "contents"));
 //connect(deleteAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(del()));

 editMenu = menuBar()->addMenu(tr("&Edit"));
 editMenu->addAction(cutAction);
 editMenu->addAction(copyAction);
 editMenu->addAction(pasteAction);
 editMenu->addAction(deleteAction);



 selectRowAction = new QAction(tr("&Row"), this);
 selectRowAction->setStatusTip(tr("Select all the cells in the "
                                  "current row"));
 //connect(selectRowAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(selectCurrentRow()));

 selectColumnAction = new QAction(tr("&Column"), this);
 selectColumnAction->setStatusTip(tr("Select all the cells in the "
                                     "current column"));
 //connect(selectColumnAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(selectCurrentColumn()));

 selectAllAction = new QAction(tr("&All"), this);
 selectAllAction->setShortcut(QKeySequence::SelectAll);
 selectAllAction->setStatusTip(tr("Select all the cells in the "
                                  "spreadsheet"));
 //connect(selectAllAction, SIGNAL(triggered()), inputWidget->getActiveSpreadsheet(), SLOT(selectAll()));

 selectSubMenu = editMenu->addMenu(tr("&Select"));
 selectSubMenu->addAction(selectRowAction);
 selectSubMenu->addAction(selectColumnAction);
 selectSubMenu->addAction(selectAllAction);
*/
}




void MainWindow::onLoginButtonClicked() {

    if (loggedIn == false) {
        numTries = 0;
        loginWindow->show();
    } else {
        loggedIn = false;
        emit logout();
    }
}

void MainWindow::onLoginSubmitButtonClicked() {

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
MainWindow::attemptLoginReturn(bool ok){

    int maxNumTries = 3;

    if (ok == true) {
        //      emit updateJobTable("");
        loginWindow->hide();
        loggedIn = true;
        loginButton->setText("Logout");
        //this->enableButtons();

        //theJobManager->up
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
MainWindow::logoutReturn(bool ok){

    if (ok == true) {
        loggedIn = false;
        loginButton->setText("Login");
        //this->disableButtons();
        // bring up login button
        //  this->onLoginButtonClicked();
    }
}

void
MainWindow::onRunButtonClicked() {
    inputWidget->onRunButtonClicked();
}

void
MainWindow::onRemoteRunButtonClicked(){
    inputWidget->onRemoteRunButtonClicked();
}
void
MainWindow::onRemoteGetButtonClicked(){
    inputWidget->onRemoteGetButtonClicked();
};

void MainWindow::onExitButtonClicked(){
    //RandomVariableInputWidget *theParameters = uq->getParameters();
    inputWidget->onExitButtonClicked();
    QCoreApplication::exit(0);
}

void
MainWindow::statusMessage(const QString msg){
    errorLabel->setText(msg);
    qDebug() << "STATUS MESSAGE" << msg;
}

void
MainWindow::errorMessage(const QString msg){
    errorLabel->setText(msg);
    qDebug() << "ERROR MESSAGE" << msg;
}

void
MainWindow::fatalMessage(const QString msg){
    errorLabel->setText(msg);
    qDebug() << "FATAL MESSAGE" << msg;
}
