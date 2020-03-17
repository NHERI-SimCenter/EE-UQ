#include <UniformMotionInput.h>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>



#include "EarthquakeRecord.h"

UniformMotionInput::UniformMotionInput(RandomVariablesContainer *theRandomVariableIW, SimCenterWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{
    QGridLayout *inputMotionLayout = new QGridLayout();
    QLabel *entryLabel = new QLabel();
    entryLabel->setText(tr("Input Motion"));

    eqMotion = new QComboBox();

    QLabel *scaleLabel = new QLabel(tr("Scale Factor"));
    scaleFactor = new QLineEdit();
    scaleFactor->setMaximumWidth(100);
    scaleFactor->setText("1.0");

    inputMotionLayout->addWidget(entryLabel,0,0);
    inputMotionLayout->addWidget(eqMotion,0,1);
    inputMotionLayout->addWidget(scaleLabel,0,3);
    inputMotionLayout->addWidget(scaleFactor,0,4);
    inputMotionLayout->setRowStretch(2,1);
    inputMotionLayout->setColumnStretch(5,1);
    // inputMotionLayout->addStretch();

    addMotion = new QPushButton("Add");
    inputMotionLayout->addWidget(addMotion,1,4);

    this->setLayout(inputMotionLayout);


    QFile file(":/images/ElCentro.json");
    if(file.open(QFile::ReadOnly)) {
       QString jsonText = QLatin1String(file.readAll());
       QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8());
       QJsonObject jsonObj = jsonDoc.object();
       EarthquakeRecord *elCentro = new EarthquakeRecord();
       elCentro->inputFromJSON(jsonObj);


       QString recordString("ElCentro");
       records.insert(std::make_pair(QString("ElCentro"), elCentro));
       eqMotion->addItem(recordString);
    }

    QFile fileR(":/images/Rinaldi.json");
    if(fileR.open(QFile::ReadOnly)) {
       QString jsonText = QLatin1String(fileR.readAll());
       QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8());
       QJsonObject jsonObj = jsonDoc.object();
       EarthquakeRecord *rinaldi = new EarthquakeRecord();
       rinaldi->inputFromJSON(jsonObj);

       QString recordString("Northridge-Rinaldi");
       records.insert(std::make_pair(QString("Northridge-Rinaldi"), rinaldi));
       eqMotion->addItem(recordString);
    }

}

bool
UniformMotionInput::inputFromJSON(QJsonObject &jsonObject)
{

    std::map<QString, EarthquakeRecord *>::iterator iter;

    // delete the earthqkaes before clear as clear does not invoke destructor if pointers
    for (iter = records.begin(); iter != records.end(); ++iter ) {
        qDebug() << "NOT DELETING OLD RECORD" << iter->second->name;
       // delete iter->second;
    }

    records.clear();
    eqMotion->clear();

    QJsonValue theValue = jsonObject["records"];
    if (theValue.isUndefined()) {
        return false;
    }
    QJsonArray theArray=theValue.toArray();

    //
    // now read records from file and populate records and comoboBox with new
    //

    for (int i=0; i<theArray.size(); i++) {
        EarthquakeRecord *theRecord = new EarthquakeRecord();
        QJsonObject theEarthquakeObj = theArray.at(i).toObject();
        theRecord->inputFromJSON(theEarthquakeObj);
        records.insert(std::make_pair(theRecord->name, theRecord));
        eqMotion->addItem(theRecord->name);
        qDebug() << "Loaded Record: " << theRecord->name;
    }

    //
    // set current record to what was saved in file
    //

    theValue = jsonObject["currentMotionIndex"];
    if (theValue.isUndefined()) {
        //errorMessage("No \"currentMotionIndex\" section");
        return false;
    }
    int currIndex = theValue.toInt();
    eqMotion->setCurrentIndex(theValue.toInt());
    theValue = jsonObject["currentMotion"];
    if (theValue.isUndefined()) {
     //   errorMessage("No \"currentMotion\" section");
        return false;
    }
    QString currentMotionName = theValue.toString();
   // qDebug() << "INDEX & NAME: " << currIndex << " " << currentMotionName;

    theValue = jsonObject["scaleFactorEQ"];//=scaleFactorEQ->currentText();
    scaleFactor->setText(theValue.toString());


        return true;
}

bool
UniformMotionInput::outputToJSON(QJsonObject &theObject)
{
    theObject["type"]="UniformMotion";

    QJsonArray motionsArray;
    int numMotions = eqMotion->count();
    std::map<QString, EarthquakeRecord *>::iterator iter;
    for (int i=0; i<numMotions; i++) {
        QString eqName = eqMotion->itemText(i);
        iter = records.find(eqName);
        if (iter != records.end()) {
            QJsonObject obj;
            EarthquakeRecord *theRecord = iter->second ;
            theRecord->outputToJSON(obj);
            motionsArray.append(obj);
        }
    }

    theObject["records"]=motionsArray;

    theObject["currentMotion"]=eqMotion->currentText();
    theObject["currentMotionIndex"]=eqMotion->currentIndex();
    theObject["scaleFactor"]= scaleFactor->text();

    return true;
}

QString
UniformMotionInput::getApplicationName(void) {
    return QString("UniformMotion");
}
