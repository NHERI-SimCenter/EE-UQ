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
  //qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  //
  // create a remote interface
  //

  QString tenant("designsafe");
  QString storage("agave://designsafe.storage.default/");

  AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage);


  //
  // create the main window
  //
  WorkflowAppWidget *theInputApp = new InputWidgetEE_UQ(theRemoteService);
  MainWindowWorkflowApp w(QString("EE-UQ: Response of Building to Earthquake"), theInputApp, theRemoteService);

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

  QFile file(":/styleCommon/style.qss");
  if(file.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
  }

  w.setStyleSheet("QLineEdit {background: #FFFFFF;}");
  w.setStyleSheet("QComboBox {background: #FFFFFF;} QLineEdit {background: #FFFFFF}");


  int res = a.exec();

  //
  // on done with event loop, logout & stop the thread
  //

  theRemoteService->logout();
  thread->quit();

  // done
  return res;
}
