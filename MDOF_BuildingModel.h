#ifndef MDOF_BUILDING_MODEL_H
#define MDOF_BUILDING_MODEL_H

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

#include <SimCenterAppWidget.h>

#include <QGroupBox>
#include <QVector>
#include <Controller2D.h>
#include <QMap>

//class QVBoxLayout;
class QLineEdit;
class InputWidgetParameters;
class RandomVariablesContainer;
class QTableWidget;


class MDOF_BuildingModel : public SimCenterAppWidget, public Controller2D
{
    Q_OBJECT
public:
    explicit MDOF_BuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent = 0);
    ~MDOF_BuildingModel();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &rvObject);
    bool inputAppDataFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString &dirName);

    void draw(GlWidget2D *);
    void getBoundary(float &height, float &width);
    void setSelectionBoundary(float y1, float y2);

signals:
    void numStoriesOrHeightChanged(int numFloors, double height);

    //    void heightChanged(double newHeight);
    //    void numFloorsChanged(int numFloors);
      
public slots:
   void clear(void);
   void setNumStoriesAndHeight(int numFloors, double height);

   //   void setNumFloors(int newNumFloors);
   //   void setHeight(double newHeight);

   void on_inFloors_editingFinished();
   void on_inWeight_editingFinished();
   void on_storyHeight_editingFinished();
   void on_inKx_editingFinished();
   void on_inKy_editingFinished();
   void on_inK_theta_editingFinished();
   //TODO: This code is not maintainable, we need to switch to Model-View Architecture
   void on_inFyx_editingFinished();
   void on_inFyy_editingFinished();
   void on_inBx_editingFinished();
   void on_inBy_editingFinished();

   // for selected floor edits
   void on_inFloorWeight_editingFinished();

   // for selected story edits
   void on_inStoryHeight_editingFinished();
   void on_inStoryKx_editingFinished();
   void on_inStoryFyx_editingFinished();
   void on_inStoryBx_editingFinished();
   void on_inStoryKy_editingFinished();
   void on_inStoryFyy_editingFinished();
   void on_inStoryBy_editingFinished();

   // for table editing
   void on_theSpreadsheet_cellChanged(int row, int column);
   void on_theSpreadsheet_cellClicked(int row, int column);

private:
    //void updateSpreadsheet();

    // QVBoxLayout *inputLayout;
    void addRandomVariable(QString &text, int numReferences = 1);
    void removeRandmVariable(QString &text, int numReferences=1);

    QLineEdit *inFloors;
    QLineEdit *inWeight;
    QLineEdit *storyHeight;
    QLineEdit *inKx;
    QLineEdit *inKy;
    QLineEdit *inK_theta;
    //TODO: This code is not maintainable, we need to switch to Model-View Architecture
    QLineEdit *inFyx;
    QLineEdit *inFyy;
    QLineEdit *inBx;
    QLineEdit *inBy;

    // selected values for selectedfloors and stories
    QGroupBox *floorMassFrame;
    QGroupBox *storyPropertiesFrame;
    QLineEdit *inFloorWeight;
    QLineEdit *inStoryHeight;
    QLineEdit *inStoryKx;
    QLineEdit *inStoryFyx;
    QLineEdit *inStoryBx;
    QLineEdit *inStoryKy;
    QLineEdit *inStoryFyy;
    QLineEdit *inStoryBy;

    QString cellText;
    bool updatingPropertiesTable;


    QList<int> dataTypes;
    //SpreadsheetWidget *theSpreadsheet;
    QTableWidget *theSpreadsheet;

    RandomVariablesContainer *theRandomVariablesContainer;
    QStringList varNamesAndValues;

    QMap<QString, int>randomVariables;

    int    numStories;
    double buildingH;

    // possible random variables
    QString storyH;
    QString floorW;
    QString Kx;
    QString Fyx;
    QString bx;
    QString Ky;
    QString Fyy;
    QString by;
    QString K_theta;

    double *floorHeights;
    double *storyHeights;

    int fMinSelected, fMaxSelected, sMinSelected,sMaxSelected,floorSelected, storySelected;
};

#endif // MDOF_BUILDING_MODEL_H
