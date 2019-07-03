#ifndef REMOTE_APPLICATION_H
#define REMOTE_APPLICATION_H

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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Purpose: a widget for submitting uqFEM jobs to HPC resource (specifically DesignSafe at moment)
//  - the widget aasks for additional info needed and provide a submit button to submit the jb when clicked.

#include <SimCenterWidget.h>
#include <Application.h>
#include <QPushButton>
#include <QJsonObject>
#include <QMap>

class RemoteService;

class QLineEdit;

class RemoteApplication : public Application
{
    Q_OBJECT
public:  
    explicit RemoteApplication(QString name, RemoteService *theRemoteService, QWidget *parent = nullptr);
    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool setupDoneRunApplication(QString &tmpDirectory, QString &inputFile);
    void setNumTasks(int numTasks);
    void setExtraInputs(QMap<QString, QString> extraInputs);
    void setExtraParameters(QMap<QString, QString> extraParameters);
    void clearExtraInputs();
    void clearExtraParameters();

signals:
   void processResults(QString &, QString &);
   void getHomeDirCall(void);
   void uploadDirCall(const QString &local, const QString &remote);
   void startJobCall(QJsonObject theJob);
   void successfullJobStart(void);

public slots:
   void onRunButtonPressed(void);
   void attemptLoginReturn(bool);
   
   void uploadDirReturn(bool);
   void getHomeDirReturned(QString);
   void startJobReturn(QString);

private:
    void submitJob(void);

    //    QLineEdit *workingDirName;
    //    QLineEdit *localAppDirName;
    //    QLineEdit *remoteAppDirName;

    QLineEdit *nameLineEdit;
    QLineEdit *numCPU_LineEdit;
    QLineEdit *numProcessorsLineEdit;
    QLineEdit *runtimeLineEdit;
    //    QLineEdit *appLineEdit;

    QString tempDirectory;
    QString remoteHomeDirPath;
    QJsonObject theJob;

    QPushButton *pushButton;

    RemoteService *theRemoteService;
    QString workflowScriptName;
    QString shortDirName;
    QMap<QString, QString> extraInputs;
    QMap<QString, QString> extraParameters;

};

#endif // REMOTE_APPLICATION_H
