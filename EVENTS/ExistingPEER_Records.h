#ifndef EXISTING_PEER_RECORDS_H
#define EXISTING_PEER_RECORDS_H

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

#include <SimCenterWidget.h>
#include <SimCenterAppWidget.h>

class RandomVariablesContainer;
class InputWidgetExistingEvent;
class QRadioButton;
class QLineEdit;
class QSpinBox;
class LineEditRV;

#include <QGroupBox>
#include <QVector>
#include <QVBoxLayout>

class PeerRecord : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit PeerRecord(RandomVariablesContainer *theRV, QWidget *parent = 0);
    ~PeerRecord();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    QRadioButton *button;  // used to mark if Event intended for deletion
    QLineEdit    *file;    // full path to file name
    QSpinBox     *dirn;
    LineEditRV    *factor;  // load factor

public slots:
    void chooseFileName(void);
    void onRemoveRecord(bool);

signals:
    void removeRecord();

private:
     RandomVariablesContainer *theRandVariableIW;
     QString lastFactor;
};

// an event can hold multiple PeerRecord, different one for diff directions
class PeerEvent : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit PeerEvent(RandomVariablesContainer *theRV, QWidget *parent = 0);
    ~PeerEvent();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    QVBoxLayout *recordLayout;

    QRadioButton *button; // used to mark if Event intended for deletion
    QLineEdit    *theName; // a QLineEdit with name of Event (filename minus path and extension)
    QVector<PeerRecord  *>theRecords;

public slots:
    void onRemoveRecord(bool);
    void onAddRecord(bool);

private:
     RandomVariablesContainer *theRandVariableIW;
};


class ExistingPEER_Records : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit ExistingPEER_Records(RandomVariablesContainer *theRandomVariableIW, QWidget *parent = 0);

    ~ExistingPEER_Records();

    bool inputFromJSON(QJsonObject &rvObject);
    bool outputToJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &rvObject);
    bool inputAppDataFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString &dirName);

public slots:
   void errorMessage(QString message);
   void addEvent(void);
   void removeEvents(void);
   void clear(void);
   void loadEventsFromDir(void);


private:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *eventLayout;

    QVector<PeerEvent *>theEvents;
    RandomVariablesContainer *theRandVariableIW;
};

#endif // EXISTING_PEER_RECORDS_H
