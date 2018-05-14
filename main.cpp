// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
