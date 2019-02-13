#include <WorkflowAppWidget.h>
#include <QWidget>
#include <RemoteService.h>

#include <QDebug>

WorkflowAppWidget::WorkflowAppWidget(RemoteService *theService, QWidget *parent)
  :QWidget(parent), theRemoteService(theService)
{

}

WorkflowAppWidget::~WorkflowAppWidget()
{

}


void
WorkflowAppWidget::setMainWindow(MainWindowWorkflowApp* window) {
    theMainWindow = window;
}


void
WorkflowAppWidget::statusMessage(const QString msg){
     qDebug() << "WorkflowAppWidget::statusMessage" << msg;
    emit sendStatusMessage(msg);
}


void
WorkflowAppWidget::errorMessage(const QString msg){
    qDebug() << "WorkflowAppWidget::errorMessage" << msg;

    emit sendErrorMessage(msg);
}


void
WorkflowAppWidget::fatalMessage(const QString msg){
  emit sendFatalMessage(msg);
}

