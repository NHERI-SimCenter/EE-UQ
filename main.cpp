
// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include <MainWindowWorkflowApp.h>
#include <QApplication>
#include <QFile>
#include <QThread>
#include <QObject>

#include <TapisV3.h>
#include <WorkflowAppEE_UQ.h>
#include <QCoreApplication>

#include <QApplication>
#include <QFile>
#include <QTime>
#include <QTextStream>
#include <GoogleAnalytics.h>
#include <QOpenGLWidget>
#include <QStandardPaths>
#include <QDir>
#include <QStatusBar>
#include <QWebEngineView>
#include <Utils/FileOperations.h>

static QString logFilePath;
static bool logToFile = false;


 // customMessgaeOutput code taken from web:
 // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output

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

#ifdef Q_OS_WIN
    QApplication::setAttribute(Qt::AA_UseOpenGLES);
#else
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif
    

    //Setting Core Application Name, Organization, and Version
    QCoreApplication::setApplicationName("EE-UQ");
    QCoreApplication::setOrganizationName("SimCenter");
    QCoreApplication::setApplicationVersion("4.1.3");    

    //Init resources from static libraries (e.g. SimCenterCommonQt or s3hark)
    Q_INIT_RESOURCE(images1);
    Q_INIT_RESOURCE(resources);

    //
    // set up logging of output messages for user debugging
    //

    logFilePath = SCUtils::getAppWorkDir();

    // full path to debug.log file
    logFilePath = logFilePath + QDir::separator() + QString("debug.log");

    // remove old log file
    QFile debugFile(logFilePath);
    debugFile.remove();

    //  check if the app is run in Qt Creator .. if not turn on logging    
    QByteArray envVar = qgetenv("QTDIR");       
    if (envVar.isEmpty())
        logToFile = true;

    qInstallMessageHandler(customMessageOutput);

    //
    // window scaling
    //

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    /******************  code to reset openGL version .. keep around in case need again
    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(glFormat);
    ***********************************************************************************/

    //
    // regular Qt startup
    //

    QApplication a(argc, argv);

    //
    // create a remote interface
    //

    QString tenant("designsafe");
    QString storage("designsafe.storage.default/");    
    QString dirName("EE-UQ");

    // AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage, &dirName);
    TapisV3 *theRemoteService = new TapisV3(tenant, storage, &dirName);    

    //
    // create the main window
    //

    WorkflowAppWidget *theInputApp = new WorkflowAppEE_UQ(theRemoteService);
    MainWindowWorkflowApp w(QString("EE-UQ: Response of Building to Earthquake"), theInputApp, theRemoteService);

    QString aboutTitle = "About the SimCenter EE-UQ Application"; // this is the title displayed in the on About dialog
    QString aboutSource = ":/resources/docs/textAboutEEUQ.html";  // this is an HTML file stored under resources
    w.setAbout(aboutTitle, aboutSource);

    QString version = QString("Version ") + QCoreApplication::applicationVersion();
    w.setVersion(version);

    QString citeText("1)Frank McKenna, Kuanshi Zhong, Michael Gardner, Adam Zsarnoczay, Sang-ri Yi, Aakash Bangalore Satish, Charles Wang, Amin Pakzad, Pedro Arduino, & Wael Elhaddad. (2024). NHERI-SimCenter/EE-UQ: Version 4.1.0 (v4.1.0). Zenodo. https://doi.org/10.5281/zenodo.13865428 \n\n2) Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, Ahsan Kareem, Wael Elhaddad, Laura Lowes, Matthew J. Schoettler, and Sanjay Govindjee (2020) A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment. Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706");
    w.setCite(citeText);

    
    QString manualURL("https://nheri-simcenter.github.io/EE-UQ-Documentation/");
    w.setDocumentationURL(manualURL);

    QString messageBoardURL("https://github.com/orgs/NHERI-SimCenter/discussions/categories/ee-uq");    
    w.setFeedbackURL(messageBoardURL);

    //
    // move remote interface to a thread
    //

    QThread *thread = new QThread();
    theRemoteService->moveToThread(thread);

    QWidget::connect(thread, SIGNAL(finished()), theRemoteService, SLOT(deleteLater()));
    QWidget::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();


    //
    // show the main window, set styles & start the event loop
    //

    w.show();
    w.statusBar()->showMessage("Ready", 5000);

#ifdef Q_OS_WIN
    QFile file(":/styleCommon/stylesheetWIN.qss");
#endif

#ifdef Q_OS_MACOS
    QFile file(":/styleCommon/stylesheetMAC.qss");
#endif

#ifdef Q_OS_LINUX
    QFile file(":/styleCommon/stylesheetMAC.qss");
#endif


    if(file.open(QFile::ReadOnly)) {
        a.setStyleSheet(file.readAll());
        qDebug() << "Stylesheet loaded: " << file.fileName();	
        file.close();
    } else {
      qDebug() << "could not open stylesheet";
    }



#ifdef _SC_RELEASE

    //Setting Google Analytics Tracking Information
    qDebug() << "Running a Release Version of EE-UQ";
    GoogleAnalytics::SetMeasurementId("G-CPFD5EFJ4Y");
    GoogleAnalytics::SetAPISecret("vxNbZfRdRUyVx3fBpdUXxg");
    GoogleAnalytics::CreateSessionId();
    GoogleAnalytics::StartSession();

    // Opening a QWebEngineView and using github to get app geographic usage
    QWebEngineView view;
    view.setUrl(QUrl("https://nheri-simcenter.github.io/EE-UQ/GA4.html"));
    view.resize(1024, 750);
    view.show();
    view.hide();
    
#endif

#ifdef _ANALYTICS

    //Setting Google Analytics Tracking Information
    qDebug() << "compiled with: ANALYTICS";
    GoogleAnalytics::SetMeasurementId("G-CPFD5EFJ4Y");
    GoogleAnalytics::SetAPISecret("vxNbZfRdRUyVx3fBpdUXxg");
    GoogleAnalytics::CreateSessionId();
    GoogleAnalytics::StartSession();
    
#endif

    //
    // exe application event-loop
    //

    int res = a.exec();


#ifdef _GA_AFTER
    
    // Opening a QWebEngineView and using github to get app geographic usage
    qDebug() << "compiled with: _GA_AFTER";
    QWebEngineView view;
    view.setUrl(QUrl("https://nheri-simcenter.github.io/EE-UQ/GA4.html"));
    view.resize(1024, 750);
    view.show();
    view.hide();
    
#endif        
    
    //
    // on done with event loop, logout & stop the thread
    //

    theRemoteService->logout();
    thread->quit();

    GoogleAnalytics::EndSession();
    // done
    return res;
}
