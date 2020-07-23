// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include <MainWindowWorkflowApp.h>
#include <QApplication>
#include <QFile>
#include <QThread>
#include <QObject>

#include <AgaveCurl.h>
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
#endif

    //Setting Core Application Name, Organization, Version and Google Analytics Tracking Id
    QCoreApplication::setApplicationName("EE-UQ");
    QCoreApplication::setOrganizationName("SimCenter");
    QCoreApplication::setApplicationVersion("2.1.0");
    GoogleAnalytics::SetTrackingId("UA-126303135-1");
    GoogleAnalytics::StartSession();
    GoogleAnalytics::ReportStart();

    //Init resources from static libraries (e.g. SimCenterCommonQt or s3hark)
    Q_INIT_RESOURCE(images1);
    Q_INIT_RESOURCE(resources);

    //
    // set up logging of output messages for user debugging
    //

    logFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
      + QDir::separator() + QCoreApplication::applicationName();

    // make sure tool dir exists in Documentss folder
    QDir dirWork(logFilePath);
    if (!dirWork.exists())
        if (!dirWork.mkpath(logFilePath)) {
            qDebug() << QString("Could not create Working Dir: ") << logFilePath;
        }

    // full path to debug.log file
    logFilePath = logFilePath + QDir::separator() + QString("debug.log");

    // remove old log file
    QFile debugFile(logFilePath);
    debugFile.remove();

    QByteArray envVar = qgetenv("QTDIR");       //  check if the app is run in Qt Creator

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
    QString storage("agave://designsafe.storage.default/");
    QString dirName("EE-UQ");

    AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage, &dirName);


    //
    // create the main window
    //

    WorkflowAppWidget *theInputApp = new WorkflowAppEE_UQ(theRemoteService);
    MainWindowWorkflowApp w(QString("EE-UQ: Response of Building to Earthquake"), theInputApp, theRemoteService);

    /*
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
        This is Version 2.1.0 of the tool and as such is limited in scope. Researchers are encouraged to comment on what additional \
        features and applications they would like to see in this application. If you want it, chances are many of your colleagues \
        also would benefit from it.\
        <p>";

        w.setAbout(textAboutEE_UQ);
    */


    QString aboutTitle = "About the SimCenter EE-UQ Application"; // this is the title displayed in the on About dialog
    QString aboutSource = ":/resources/docs/textAboutEEUQ.html";  // this is an HTML file stored under resources
    w.setAbout(aboutTitle, aboutSource);

    QString version("Version 2.1.0");
    w.setVersion(version);

    QString citeText("Frank McKenna, Wael Elhaddad, Michael Gardner, Adam Zsarnoczay, & Charles Wang. (2019, October 8). NHERI-SimCenter/EE-UQ: Version 2.0.0 (Version v2.0.0). Zenodo. http://doi.org/10.5281/zenodo.3475642");
    w.setCite(citeText);

    QString manualURL("https://nheri-simcenter.github.io/EE-UQ-Documentation/");
    w.setDocumentationURL(manualURL);

    QString messageBoardURL("https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=6.0");
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
        file.close();
    } else {
        qDebug() << "could not open stylesheet";
    }


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
