#include <WorkflowAppWidget.h>
#include <QWidget>
#include <RemoteService.h>


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
    emit sendStatusMessage(msg);
}


void
WorkflowAppWidget::errorMessage(const QString msg){
    emit sendErrorMessage(msg);
}


void
WorkflowAppWidget::fatalMessage(const QString msg){
  emit sendFatalMessage(msg);
}

