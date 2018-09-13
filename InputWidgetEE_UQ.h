#ifndef INPUT_WIDGET_EE_UQ_H
#define INPUT_WIDGET_EE_UQ_H

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

#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHBoxLayout>
#include "MainWindow.h"

class RandomVariableInputWidget;
//class InputWidgetSheetSIM;
class InputWidgetBIM_Selection;
class InputWidgetSampling;
class EarthquakeLoadingInput;
class InputWidgetOpenSeesAnalysis;
class UQOptions;
class ResultsWidget;
class GeneralInformationWidget;
class InputWidgetEarthquakeEvent;
class QStackedWidget;
class DakotaResults;

class RunLocalWidget;
class RunWidget;
class Application;
class RemoteService;
class RemoteJobManager;

class InputWidgetEE_UQ : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidgetEE_UQ(RemoteService *theService, QWidget *parent = 0);
    ~InputWidgetEE_UQ();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    void clear(void);

    void setMainWindow(MainWindow* window);
    void onRunButtonClicked();
    void onRemoteRunButtonClicked();
    void onRemoteGetButtonClicked();
    void onExitButtonClicked();
    
signals:
    void setUpForApplicationRunDone(QString &tmpDirectory, QString &inputFile);
    void sendLoadFile(QString filename);

    void sendStatusMessage(QString message);
    void sendErrorMessage(QString message);
    void sendFatalMessage(QString message);

public slots:  
    void selectionChangedSlot(const QItemSelection &, const QItemSelection &);

    void setUpForApplicationRun(QString &, QString &);
    void processResults(QString &dakotaOut, QString &dakotaTab);

    void loadFile(QString filename);
    void statusMessage(QString message);
    void errorMessage(QString message);
    void fatalMessage(QString message);


private:

    MainWindow* window;

    QHBoxLayout *horizontalLayout;
    QTreeView *treeView;
    QStandardItemModel *standardModel;

    GeneralInformationWidget *theGI;
    RandomVariableInputWidget *theRVs;

    //InputWidgetSheetSIM *theSIM;
    InputWidgetBIM_Selection *theSIM;
    InputWidgetSampling *theUQ;
    InputWidgetEarthquakeEvent *theEvent;
    InputWidgetOpenSeesAnalysis *theAnalysis;
    DakotaResults *theResults;
   // RunLocalWidget *theRunLocalWidget;

    RemoteService *theRemoteService;
    RunWidget *theRunWidget;
    Application *localApp;
    Application *remoteApp;
    RemoteJobManager *theJobManager;

    QModelIndex infoItemIdx;
    SimCenterWidget  *currentWidget;
    QJsonObject *jsonObjOrig;

    QStackedWidget *theStackedWidget;
};

#endif // INPUT_WIDGET_EE_UQ_H
