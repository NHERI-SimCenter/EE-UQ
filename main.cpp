// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include "MainWindow.h"
#include <QApplication>
#include <QFile>
#include <QThread>
#include <QObject>

#include <AgaveCurl.h>

int main(int argc, char *argv[])
{
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
  MainWindow w(theRemoteService);

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

  /*
  QFile file(":/styleCommon/style.qss");
  if(file.open(QFile::ReadOnly)) {
     QString styleSheet = QLatin1String(file.readAll());
     a.setStyleSheet(styleSheet);
  }
  */
  int res = a.exec();

  //
  // on done with event loop, logout & stop the thread
  //

  theRemoteService->logout();
  thread->quit();

  // done
  return res;
}
