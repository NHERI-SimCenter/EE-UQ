#ifndef WORKFLOW_APP_WIDGET_H
#define WORKFLOW_APP_WIDGET_H

/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include <QWidget>

#include "MainWindow.h"

class MainWindowWorkflowApp;
class RemoteService;


class WorkflowAppWidget : public QWidget
{
    Q_OBJECT
public:
    WorkflowAppWidget(RemoteService *theService, QWidget *parent = 0);
    virtual ~WorkflowAppWidget();

    void setMainWindow(MainWindowWorkflowApp* window);

    virtual bool outputToJSON(QJsonObject &rvObject) =0;
    virtual bool inputFromJSON(QJsonObject &rvObject) =0;
    virtual void clear(void) =0;

    virtual void onRunButtonClicked() =0;
    virtual void onRemoteRunButtonClicked() =0;
    virtual void onRemoteGetButtonClicked() =0;
    virtual void onExitButtonClicked() =0;
    virtual int getMaxNumParallelTasks() =0;
    
signals:
    void setUpForApplicationRunDone(QString &tmpDirectory, QString &inputFile);
    void sendLoadFile(QString filename);

    void sendStatusMessage(QString message);
    void sendErrorMessage(QString message);
    void sendFatalMessage(QString message);

public slots:  
    //virtual void selectionChangedSlot(const QItemSelection &, const QItemSelection &) =0;

    virtual void setUpForApplicationRun(QString &, QString &) =0;
    virtual void processResults(QString dakotaOut, QString dakotaTab, QString inputFile) =0;

    virtual void loadFile(QString filename) =0;
    void statusMessage(QString message);
    void errorMessage(QString message);
    void fatalMessage(QString message);

protected:
    MainWindowWorkflowApp *theMainWindow;
    RemoteService *theRemoteService;
};

#endif // WORKFLOW_APP_WIDGET_H
