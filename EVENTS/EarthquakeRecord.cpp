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

#include "EarthquakeRecord.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <QString>

EarthquakeRecord::EarthquakeRecord()
{
     data = 0;
     scaleFactor = 1.0;
}

EarthquakeRecord::EarthquakeRecord(QString fileName)
{

}

EarthquakeRecord::EarthquakeRecord(QString theName, int numberSteps, double theDt, double *theData)
    :name(theName), numSteps(numberSteps), dt(theDt), data(0)
{
    data = new double[numSteps];
    for (int i=0; i<numSteps; i++)
    data[i] = theData[i];
}

EarthquakeRecord::~EarthquakeRecord()
{
    if (data != 0)
        delete [] data;
}

double
EarthquakeRecord::getScaleFactor(void){
    return scaleFactor;
}

void
EarthquakeRecord::setScaleFactor(double newFactor){
    scaleFactor = newFactor;
}

void
EarthquakeRecord::outputToJSON(QJsonObject &jsonObj){
    jsonObj["name"]=name;
    jsonObj["dT"]=dt;
    jsonObj["numPoints"]=numSteps;
    jsonObj["scaleFactor"]=scaleFactor;
    QJsonArray dataValues;
    for (int i=0; i<numSteps; i++) {
        dataValues.append(data[i]);
    }
    jsonObj["accel_data"]=dataValues;
}

int
EarthquakeRecord::inputFromJSON(QJsonObject &jsonObj){

    QJsonValue theValue = jsonObj["name"];
    if (theValue.isNull() || theValue.isUndefined()) {
        return -1;
    }
    name=theValue.toString();

    // get dT, return error if not there
    theValue = jsonObj["dT"];
    if (theValue.isNull() || theValue.isUndefined()) {
        return -1;
    }
    dt=theValue.toDouble();


    theValue = jsonObj["numPoints"];
    if (theValue.isNull() || theValue.isUndefined()) {
        return -1;
    }


    numSteps=theValue.toInt();
     qDebug() << "numPoints: " << numSteps;
    data = new double[numSteps];

    theValue = jsonObj["scaleFactor"];
    if (theValue.isUndefined())
        theValue = 1.0;
    else if (!theValue.isNull()) {
        scaleFactor=theValue.toDouble();
    } else
       scaleFactor = 1.0;

    if (data != 0)
        delete [] data;

    theValue = jsonObj["accel_data"];
    if (theValue.isNull() || theValue.isUndefined()) {
        return -1;
    }

    QJsonArray dataPoints = theValue.toArray();
    for (int i=0; i<numSteps; i++)
        data[i] = dataPoints.at(i).toDouble();

    return 0;
}
