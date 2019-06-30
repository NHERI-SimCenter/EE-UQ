// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include <MainWindowWorkflowApp.h>
#include <QApplication>
#include <QFile>
#include <QThread>
#include <QObject>

#include <AgaveCurl.h>
#include <InputWidgetEE_UQ.h>


#include <QApplication>
#include <QFile>
#include <QTime>
#include <QTextStream>
#include <GoogleAnalytics.h>

 // customMessgaeOutput code from web:
 // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output

const QString logFilePath = "debug.log";
bool logToFile = false;

void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "Debug"}, {QtInfoMsg, "Info"}, {QtWarningMsg, "Warning"}, {QtCriticalMsg, "Critical"}, {QtFatalMsg, "Fatal"}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    if (logToFile) {
        QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
        QFile outFile(logFilePath);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
    } else {
        fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
        fflush(stderr);
    }

    if (type == QtFatalMsg)
        abort();
}


int main(int argc, char *argv[])
{
    //Setting Core Application Name, Organization, Version and Google Analytics Tracking Id
    QCoreApplication::setApplicationName("EE-UQ");
    QCoreApplication::setOrganizationName("SimCenter");
    QCoreApplication::setApplicationVersion("1.2.0");
    GoogleAnalytics::SetTrackingId("UA-126303135-1");
    GoogleAnalytics::StartSession();
    GoogleAnalytics::ReportStart();

  //
  // set up logging of output messages for user debugging
  //

  // remove old log file
  QFile debugFile("debug.log");
  debugFile.remove();

  QByteArray envVar = qgetenv("QTDIR");       //  check if the app is run in Qt Creator
  
  if (envVar.isEmpty())
    logToFile = true;
  
  qInstallMessageHandler(customMessageOutput);

  // window scaling for mac
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  //
  // create a remote interface
  //

  QString tenant("designsafe");
  QString storage("agave://designsafe.storage.default/");
  QString dirName("EE-UQ");

  AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage, &dirName);


  //
  // create the main window
  //
  WorkflowAppWidget *theInputApp = new InputWidgetEE_UQ(theRemoteService);
  MainWindowWorkflowApp w(QString("EE-UQ: Response of Building to Earthquake"), theInputApp, theRemoteService);


  QString textAboutEE_UQ = "\
          <p> \
          This is the Earthquake Engineering with Uncertainty Quantification (EE-UQ) application.\
          <p>\
          This open-source research application, https://github.com/NHERI-SimCenter/EE-UQ, provides an application \
      researchers can use to predict the response of a building to earthquake events. The application is focused on \
      quantifying the uncertainties in the predicted response, given the that the properties of the buildings and the earthquake \
      events are not known exactly, and that the simulation software and the user make simplifying assumptions in the numerical \
      modeling of that structure. In the application the user is required to characterize the uncertainties in the input, the \
      application will after utilizing the selected sampling method, provide information that characterizes the uncertainties in \
      the response. The computations to make these determinations can be prohibitively expensive. To overcome this impedement the \
      user has the option to perform the computations on the Stampede2 supercomputer. Stampede2 is located at the Texas Advanced \
      Computing Center and is made available to the user through NHERI DesignSafe, the cyberinfrastructure provider for the distributed \
      NSF funded Natural Hazards in Engineering Research Infrastructure, NHERI, facility.\
      <p>\
      The computations are performed in a workflow application. That is, the numerical simulations are actually performed by a \
      number of different applications. The EE-UQ backend software runs these different applications for the user, taking the \
      outputs from some programs and providing them as inputs to others. The design of the EE-UQ application is such that \
      researchers are able to modify the backend application to utilize their own application in the workflow computations. \
      This will ensure researchers are not limited to using the default applications we provide and will be enthused to provide\
      their own applications for others to use.\
      <p>\
      This is Version 1.2.0 of the tool and as such is limited in scope. Researchers are encouraged to comment on what additional \
      features and applications they would like to see in this application. If you want it, chances are many of your colleagues \
      also would benefit from it.\
      <p>";

     w.setAbout(textAboutEE_UQ);

     QString version("Version 1.2.0");
     w.setVersion(version);

     QString citeText("Frank McKenna, Wael Elhaddad, Adam Zsarnoczay, Michael Gardner, & Charles Wang. (2019, June 29). NHERI-SimCenter/EE-UQ: Release v1.2.0 (Version v1.2.0). Zenodo. http://doi.org/10.5281/zenodo.3262287");
  w.setCite(citeText);

     QString manualURL("https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community//SimCenter/Software/EE_UQ");
     w.setDocumentationURL(manualURL);
 
  //
  // move remote interface to a thread
  //

  QThread *thread = new QThread();
  theRemoteService->moveToThread(thread);

  QWidget::connect(thread, SIGNAL(finished()), theRemoteService, SLOT(deleteLater()));
  QWidget::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

  thread->start();

  //
  // show the main window & start the event loop
  //

  w.show();


  QFile file(":/styleCommon/common_experimental.qss");
  QFile fileEEUQ(":/styles/stylesheet_eeuq.qss");
  if(file.open(QFile::ReadOnly) && fileEEUQ.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    QString styleSheetEEUQ = QLatin1String(fileEEUQ.readAll());
    a.setStyleSheet(styleSheet+styleSheetEEUQ);
    file.close();
    fileEEUQ.close();
  }



/*
  theInputApp->setStyleSheet("QComboBox {background: #FFFFFF;} \
QGroupBox {font-weight: bold;}\
QLineEdit {background-color: #FFFFFF; border: 2px solid darkgray;} \
QTabWidget::pane {background-color: #ECECEC; border: 1px solid rgb(239, 239, 239);}");
*/


//QTQTabWidget::pane{
//                             border: 1px solid rgb(239, 239, 239);
//                             background: rgb(239, 239, 239);
//                         }abWidget::pane {background-color: #ECECEC;}");

  int res = a.exec();

  //
  // on done with event loop, logout & stop the thread
  //

  theRemoteService->logout();
  thread->quit();

  GoogleAnalytics::EndSession();
  // done
  return res;
}
