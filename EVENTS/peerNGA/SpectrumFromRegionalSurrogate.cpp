#include "SpectrumFromRegionalSurrogate.h"
#include <QTableView>
#include <QGridLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include "surrogateGpParser.h"
#include <QJsonDocument>
#include <QGroupBox>
#include <QComboBox>
#include <QJsonObject>
#include <QRegularExpression>
#include <QFileInfo>
#include <sstream>
#include <SimCenterPreferences.h>
#include <QCoreApplication>
#include <QProcess>
#include <QProcessEnvironment>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

SpectrumFromRegionalSurrogate::SpectrumFromRegionalSurrogate(QWidget* parent): AbstractTargetWidget(parent)
{
    auto layout = new QGridLayout(this);
    // surrogagte json file
    QLabel *label1 = new QLabel("Hazard SurrogateGP Info (.json)");
    inputScript = new QLineEdit;
    QPushButton *chooseMS_Button = new QPushButton();
    chooseMS_Button->setText(tr("Choose"));
    connect(chooseMS_Button, &QPushButton::clicked, this, [this](){
        QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"),
                              "",
                              "Json files (*.json)");
        if(!fileName.isEmpty()) {
            this->setMainScript(fileName);
        }
    });
    connect(inputScript, SIGNAL(textChanged(QString)),SLOT(showGpOptions(QString)));
    layout->addWidget(label1,0,0);
    layout->addWidget(inputScript,0,1);
    layout->addWidget(chooseMS_Button,0,2);

    // surrogate pkl file
    QLabel *label2 = new QLabel("Hazard SurrogateGP Model (.pkl)");
    postprocessScript = new QLineEdit;
    // KZ: .pkl is not optional.
    //postprocessScript->setPlaceholderText("(Optional)");
    QPushButton *choosePostprocessScript = new QPushButton();
    choosePostprocessScript->setText(tr("Choose"));
    connect(choosePostprocessScript, &QPushButton::clicked, this, [this](){
      QString selectedFile = QFileDialog::getOpenFileName(this,
                              tr("Postprocess Script"),
                              "",
                              "Pickle files (*.pkl)");

        if(!selectedFile.isEmpty()) {
            postprocessScript->setText(selectedFile);
        }
    });
    groupBox = new QGroupBox("Options");
    QGridLayout *optionsLayout = new QGridLayout(groupBox);
    QHBoxLayout * labelVarThresLayout = new QHBoxLayout();
    thresVal = new QLineEdit("0");
    thresVal->setMaximumWidth(100);
    thresVal->setValidator(new QDoubleValidator);
    connect(thresVal,SIGNAL(textEdited(QString)), this, SLOT(updateMessage(QString)));

    labelVarThresLayout->addWidget( new QLabel("Maximum Allowable Normalized Variance  "));
    labelVarThresLayout->addWidget(thresVal);
    labelVarThresLayout->addStretch(1);
    QLabel *optionsLabel = new QLabel("When surrogate model gives imprecise prediction at certain sample locations");
    option1Button = new QRadioButton("Stop Analysis");
    option2Button = new QRadioButton("Ignore and Continue");
    option3Button = new QRadioButton("Run Exact FEM Simulation");
    labelThresMsg = new QLabel(" ");
    labelThresMsg->setStyleSheet("color: red");
    option2Button->setChecked(true);
    QHBoxLayout * gpOutputLayout = new QHBoxLayout();
    gpOutputComboBox= new QComboBox;
    gpOutputComboBox->addItem("Median (representative) prediction");
    gpOutputLayout->addWidget( new QLabel("GP output   "));
    gpOutputLayout->addWidget(gpOutputComboBox);
    gpOutputLayout->addStretch(1);
    connect(option3Button, &QRadioButton::toggled, this, [=](bool tog){
        if (tog==false)
        {
            //femWidget->setVisible(false);
        } else {
            //femWidget->setVisible(true);
        }
    });
    qoiNames = new QLabel();
    qoiNames->setTextInteractionFlags(Qt::TextSelectableByMouse);
    optionsLayout->addLayout(labelVarThresLayout, 0,0,1,-1);
    //optionsLayout->addWidget(thresVal,0,4);
    //optionsLayout->addWidget(labelThresMsg,1,0,1,-1);
    //optionsLayout->addWidget(optionsLabel, 2,0,1,-1);
    //optionsLayout->addWidget(option1Button, 3,0,1,-1);
    //optionsLayout->addWidget(option2Button, 4,0,1,-1);
    //optionsLayout->addWidget(option3Button, 5,0,1,-1);
    //optionsLayout->addWidget(femWidget, 6,0);
    optionsLayout->addWidget(qoiNames, 1,0);
    optionsLayout->addLayout(gpOutputLayout, 2,0,1,-1);
    optionsLayout->setColumnStretch(5,1.0);
    //optionsLayout->setSpacing(0);
    //optionsLayout->setHorizontalSpacing(0);
    //optionsLayout->setContentsMargins(0,0,0,0);
    layout->addWidget(label2,1,0);
    layout->addWidget(postprocessScript,1,1);
    layout->addWidget(choosePostprocessScript,1,2);
    layout->addWidget(groupBox,2,0,1,3);
    groupBox->setVisible(false);
    layout->setRowStretch(3,1.0);
    layout->setColumnStretch(1,4);
    layout->setColumnStretch(4,1);

    // periods for the spectrum
    QLabel *periodLabel = new QLabel(tr("Intensity Measure Periods (sec):"),this);
    // Set a validator to allow only numbers, periods, and spaces
    QRegularExpression regExpAllow("^([1-9][0-9]*|[1-9]*\\.[0-9]*|0\\.[0-9]*)*(([ ]*,[ ]*){0,1}([[1-9]*\\.[0-9]*|[1-9][0-9]*|0\\.[0-9]*))*");
    LEValidator = new QRegularExpressionValidator(regExpAllow,this);
    periodsLineEdit = new QLineEdit(this);
    periodArray = {};
    connect(this->periodsLineEdit, &QLineEdit::textChanged, this, &SpectrumFromRegionalSurrogate::checkPeriodsValid);
    connect(this->periodsLineEdit, &QLineEdit::editingFinished, this, &SpectrumFromRegionalSurrogate::commitPeriods);
    layout->addWidget(periodLabel, 3, 0, 1, 1);
    layout->addWidget(periodsLineEdit, 3, 1, 1, 3);

    // input random variables
    vLayout = new QVBoxLayout();
    rvLayout = new QVBoxLayout();
    rvBox = new QGroupBox("Input Random Variables");
    rvBox->setLayout(rvLayout);
    //rvLayout->addStretch();
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setWidget(rvBox);
    sa->setMinimumHeight(100);
    sa->setMaximumHeight(200);
    vLayout->addWidget(sa);
    layout->addLayout(vLayout,4,0,1,3);

    // add button
    QPushButton *runButton = new QPushButton("Get Spectrum");
    layout->addWidget(runButton, 5, 0, 1, 3);
    connect(runButton, SIGNAL(clicked(bool)), this, SLOT(getSpectrum(void)));
    connect(this, SIGNAL(runComplete(bool, QString, QString)), this, SLOT(saveSpectrum(bool, QString, QString)));

    this->setLayout(layout);

    /***
    auto table = new QTableView(this);
    model = new UserSpectrumModel(this);
    table->setModel(model);
    layout->addWidget(table, 0, 0, 1, 3);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto addButton = new QPushButton("Add");
    layout->addWidget(addButton, 1, 0);
    connect(addButton, &QPushButton::clicked, this, [this, table](){
        model->addPoint(table->currentIndex().row());
    });

    auto removeButton = new QPushButton("Remove");
    layout->addWidget(removeButton, 1, 1);
    connect(removeButton, &QPushButton::clicked, this, [this, table](){
        model->removePoint(table->currentIndex().row());
    });

    auto loadButton = new QPushButton("Load CSV");
    layout->addWidget(loadButton, 1, 2);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadCSV()));
    ***/

}

QJsonObject SpectrumFromRegionalSurrogate::serialize() const
{
    QJsonObject json;

    // adding more surrogate info. to the jsonObject
    json["Application"] = QString("SurrogateGP");
    QJsonObject fem_data;
    QFileInfo ms_path(inputScript->text());
    QFileInfo ps_path(postprocessScript->text());
    QDir tmp1 = ms_path.absoluteDir();
    fem_data["MS_Path"] = tmp1.absolutePath();
    //qDebug() << "MS_Path: " << tmp1.absolutePath() << "\n";
    QDir tmp2 = ps_path.absoluteDir();
    fem_data["PS_Path"] = tmp2.absolutePath();
    //qDebug() << "PS_Path: " << tmp2.absolutePath() << "\n";
    fem_data["mainScript"] = ms_path.fileName();
    fem_data["postprocessScript"] = ps_path.fileName();
    json["ApplicationData"] = fem_data;
    json["femOption"] = "continue";
    json["gpSeed"] = 1;
    json["predictionOption"] = gpOutputComboBox->currentText();
    json["varThres"] = thresVal->text().toDouble();
    json["periods"] = periodsLineEdit->text();
    QJsonArray outputRVValues;
    for (auto & curVal : rvValues) {
        outputRVValues.append(curVal);
    }
    QJsonArray outputRVNames;
    for (auto & curName : rvNames) {
        outputRVNames.append(curName);
    }
    json["rvNames"] = outputRVNames;
    json["rvValues"] = outputRVValues;

    return json;
}

void SpectrumFromRegionalSurrogate::deserialize(const QJsonObject &json)
{
    // read main and post-process scripts
    QJsonObject fem_data;
    fem_data = json["ApplicationData"].toObject();
    auto ms_path = fem_data["MS_Path"].toString() + QDir().separator() + fem_data["mainScript"].toString();
    //qDebug() << "ms_path: " << ms_path << "\n";
    inputScript->setText(ms_path);
    auto ps_path = fem_data["PS_Path"].toString() + QDir().separator() + fem_data["postprocessScript"].toString();
    //qDebug() << "ps_path: " << ps_path << "\n";
    postprocessScript->setText(ps_path);

    // set main script
    this->setMainScript(ms_path);

    // set up GP parameters
    gpOutputComboBox->setCurrentIndex(0);
    thresVal->setText(QString::number(json["varThres"].toDouble()));

    // set up periods
    periodsLineEdit->setText(json["periods"].toString());
    this->commitPeriods();
    //qDebug() << "periodArray: " << periodArray;

    // set up RVs
    auto inputRVNames = json["rvNames"].toArray().toVariantList();
    auto inputRVValues = json["rvValues"].toArray().toVariantList();
    for (int j = 0; j < inputRVNames.size(); j++) {
        auto curName = inputRVNames[j].toString();
        for (int i = 0; i < rvNames.size(); i++) {
            if (rvNames[i].compare(curName)==0) {
                rvValues[i] = inputRVValues[j].toDouble();
                rvValueLE[i]->setText(inputRVValues[j].toString());
                break;
            }
        }
    }

    /***
    QList<QPair<double, double>> spectrum;
    for (auto point: json["UserSpectrum"].toArray())
        spectrum.append({point.toArray()[0].toDouble(), point.toArray()[1].toDouble()});

    model->set(spectrum);
    ***/
}

QList<QPair<double, double>> SpectrumFromRegionalSurrogate::spectrum() const
{
    return model->spectrum();
}

void SpectrumFromRegionalSurrogate::loadCSV(void)
{
    // loading a csv table of user-specified spectrum
    spectrumfilepath=QFileDialog::getOpenFileName(this,tr("Spectrum CSV"));
    if(spectrumfilepath.isEmpty())
    {
        spectrumfilepath = "NULL";
        qDebug()<<"Path is empty.";
        return;
    }
    this->loadSpectrum();

}

void SpectrumFromRegionalSurrogate::loadSpectrum(void)
{
    qDebug()<<"Loading spectrum file...";
    QFile tmpFile(spectrumfilepath);
    if (!tmpFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Cannot find the file: " + spectrumfilepath + "\nCheck your directory and try again.";
        return;
    }
    QStringList rowLines;
    while (!tmpFile.atEnd())
    {
        QString line = tmpFile.readLine();
        rowLines << line;
    }
    auto numRows = rowLines.size();
    if(numRows == 0)
    {
        qDebug()<<"Error in parsing the .csv file "<<tmpFile.fileName() <<" in UserDefinedDatabase::parseCSV file";
        return;
    }
    int row_num = 0;
    QList<QPair<double,double>> tmpSpectrum;
    QString separator = ",";
    QStringList tmpData;
    for(auto&& it: rowLines)
    {
        if (row_num > 0)
        {
            tmpData = it.split(separator);
            if (tmpData.size() == 1)
            {
                // try spacings
                separator = "\\s+";
                tmpData = it.split(QRegularExpression(separator));
            }
            auto tmpT = tmpData.at(0).toDouble();
            auto tmpSa = tmpData.at(1).toDouble();
            // validating
            if (tmpT < 0.0 || tmpSa < 0.0)
            {
                qDebug()<<"Negative values in the spectrum.";
                break;
            }
            tmpSpectrum.append({tmpT, tmpSa});
        }
        row_num = row_num+1;
    }
    // setting the spectrum
    model->set(tmpSpectrum);

}

void SpectrumFromRegionalSurrogate::setMainScript(QString name1){

    // remove old random variables
    QStringList names;
    for (int i=0; i<varNamesAndValues.size()-1; i+=2) {
        names.append(varNamesAndValues.at(i));
    }

    //RandomVariablesContainer *theRVs= RandomVariablesContainer::getInstance();
    //theRVs->removeRandomVariables(names);

    // set file name & ebtry in qLine edit

    inputScript->setText(name1);

    //
    // parse file for random variables and add them
    //

    surrogateGpParser theParser;
    varNamesAndValues = theParser.getVariables(name1);
    this->showRVs(varNamesAndValues);
    //theRVs->addRVsWithValues(varNamesAndValues);

    return;
}

void SpectrumFromRegionalSurrogate::showGpOptions(QString name1) {

    option2Button->setChecked(true);

    QFile file(name1);
    QString appName, mainScriptDir,postScriptDir;

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString val;
        val=file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonSur = doc.object();
        file.close();

        auto GPidentifier = jsonSur.find("kernName"); // should be right .json file
        QJsonObject jsonHif = jsonSur["highFidelityInfo"].toObject();
        if (!jsonSur.isEmpty() && GPidentifier != jsonSur.end()) {
            bool from_data=!jsonHif["doSimulation"].toBool();
            QJsonArray RVArray = jsonSur["randomVariables"].toArray();
            QJsonArray QoIArray = jsonSur["ylabels"].toArray();

            foreach (const QJsonValue & v, QoIArray){
                 qoiNames_list.push_back(v.toString());
            }
            qoiNames->setText(QString("Intensity Measures: ") + qoiNames_list.join(", "));

            if (from_data) {
                isData = true;
                appName = "data";
                option3Button->setDisabled(true);
                thresVal->setText("0.02");
                groupBox->setVisible(true);
            } else {
                isData = false;
                option3Button->setDisabled(false);
                QJsonObject jsonPred = jsonSur["predError"].toObject();
                QJsonArray precArray = jsonPred["percent"].toArray();
                QJsonArray valsArray = jsonPred["value"].toArray();
                //QJsonObject jsonFEM = jsonSur["fem"].toObject();
                // interpolate
                QVector<double> percVal_tmp, thrsVal_tmp;
                foreach (const QJsonValue & v, precArray)
                     percVal_tmp.push_back(v.toDouble());
                foreach (const QJsonValue & v, valsArray)
                     thrsVal_tmp.push_back(v.toDouble());
                percVals=percVal_tmp;
                thrsVals=thrsVal_tmp;
                thresVal->setText(QString::number(this->interpolateForGP(percVal_tmp,thrsVal_tmp,0.5)));
                this->updateMessage( thresVal->text());
                // save names
                //appName = jsonFEM["program"].toString();
                //mainScriptDir = jsonFEM["inputFile"].toString();
                //postScriptDir = jsonFEM["postprocessScript"].toString();
                //labelProgName->setText("      • Application Name: " + appName);
                //labelProgDir1->setText("      • Main Script: "+ mainScriptDir);
                //labelProgDir2->setText("      • Postprocess Script: "+ postScriptDir );
                groupBox->setVisible(true);
            }
        } else {
            appName = "NA";
            thresVal->setText("0");
            groupBox->setVisible(false);
        }
    } else {
        appName = "NA";
        thresVal->setText("0");
        groupBox->setVisible(false);
    }
}

void SpectrumFromRegionalSurrogate::updateMessage(QString textval){
    double thres=textval.toDouble();
    double percEst = this->interpolateForGP(thrsVals,percVals,thres)*100;
    if (thres>thrsVals[thrsVals.size()-1]) {
        percEst=percVals[percVals.size()-1]*100;
    } else if (thres<thrsVals[0]) {
        percEst=percVals[0]*100;
    }
    if (!isData) {
        labelThresMsg->setVisible(true);
        labelThresMsg->setText("Note: around " + QString::number(percEst, 'f', 1) + "% of new samples in training range will exceed the tolerance limit.");
    } else {
        labelThresMsg->setVisible(false);
    }
};

double SpectrumFromRegionalSurrogate::interpolateForGP(QVector<double> X, QVector<double> Y, double Xval){
    int N = X.count();

    if (X.size()==1) {
        return 0; // make it %
    }

    double estY=0;
    if (X[0]<X[1]) {
        for (int np=0; np<N; np++) {
            if(X[np] > Xval) {
                if (np==0)
                    estY = X[0];
                else
                    estY = Y[np-1]+(Y[np]-Y[np-1])*(Xval-X[np-1])/(X[np]-X[np-1]);
                break;
            }
        }
    } else {
        for (int np=N-1; np>-1; np--) {
            if(X[np] > Xval) {
                if (np==0)
                    estY = X[0];
                else
                    estY = Y[np-1]+(Y[np]-Y[np-1])*(Xval-X[np-1])/(X[np]-X[np-1]);
                break;
            }
        }
    }

    return (estY);
}

/***
bool SpectrumFromRegionalSurrogate::outputToJSON(QJsonObject &jsonObject) {
    bool result = true;
    qDebug() << QString("outputToJSON for SpectrumFromRegionalSurrogate configuration\n");

    // adding more surrogate info. to the jsonObject
    jsonObject["Application"] = QString("SurrogateGP");
    QJsonObject fem_data;
    QFileInfo ms_path(inputScript->text());
    QFileInfo ps_path(postprocessScript->text());
    QDir tmp1 = ms_path.absoluteDir();
    fem_data["MS_Path"] = tmp1.absolutePath();
    QDir tmp2 = ps_path.absoluteDir();
    fem_data["PS_Path"] = tmp2.absolutePath();
    fem_data["mainScript"] = ms_path.fileName();
    fem_data["postprocessScript"] = ps_path.fileName();
    jsonObject["ApplicationData"] = fem_data;
    jsonObject["femOption"] = "continue";
    jsonObject["gpSeed"] = 1;
    jsonObject["predictionOption"] = gpOutputComboBox->currentText();
    jsonObject["varThres"] = thresVal->text().toDouble();
    jsonObject["periods"] = periodsLineEdit->text();
    QJsonArray outputRVValues;
    for (auto & curVal : rvValues) {
        outputRVValues.append(curVal);
    }
    QJsonArray outputRVNames;
    for (auto & curName : rvNames) {
        outputRVNames.append(curName);
    }
    jsonObject["rvNames"] = outputRVNames;
    jsonObject["rvValues"] = outputRVValues;

    return result;
}
***/

bool SpectrumFromRegionalSurrogate::writeConfigJSON(QJsonObject &configObject) {

    bool result = true;
    QJsonArray rvArray;

    QJsonObject fem;
    fem["Application"] = QString("SurrogateGP");
    QJsonObject fem_data;
    QFileInfo ms_path(inputScript->text());
    QFileInfo ps_path(postprocessScript->text());
    QDir tmp1 = ms_path.absoluteDir();
    fem_data["MS_Path"] = tmp1.absolutePath();
    QDir tmp2 = ps_path.absoluteDir();
    fem_data["PS_Path"] = tmp2.absolutePath();
    fem_data["mainScript"] = ms_path.fileName();
    fem_data["postprocessScript"] = ps_path.fileName();
    fem["ApplicationData"] = fem_data;
    fem["femOption"] = "continue";
    fem["gpSeed"] = 1;
    fem["predictionOption"] = gpOutputComboBox->currentText();
    fem["varThres"] = thresVal->text().toDouble();

    QJsonObject uq;
    uq["Application"] = QString("SimCenter-UQ");
    QJsonObject uq_data;
    uq["ApplicationData"] = uq_data;

    QJsonObject app;
    app["FEM"] = fem;
    app["UQ"] = uq;
    configObject["Applications"] = app;

    QJsonArray edp;
    for (int i = 0; i < qoiNames_list.size(); ++i) {
        QJsonObject tmp;
        tmp["length"] = 1;
        tmp["name"] = qoiNames_list.at(i);
        tmp["type"] = QString("salar");
        edp.append(tmp);
    }
    configObject["EDP"] = edp;

    QJsonObject fem2;
    configObject["FEM"] = fem2;

    QJsonObject uq_method;
    uq_method["RVdataGroup"] = "";
    QJsonObject sampling;
    sampling["method"] = QString("LHS");
    sampling["samples"] = 1;
    sampling["seed"] = 100; // fixed here
    uq_method["samplingMethodData"] = sampling;
    uq_method["uqEngine"] = "SimCenterUQ";
    uq_method["uqType"] = "Forward Propagation";
    configObject["UQ_Method"] = uq_method;

    configObject["WorkflowType"] = QString("Building Simulation");
    configObject["isSurrogate"] =0;
    configObject["resultType"] = QString("SimCenterUQResultSampling");

    QJsonArray rvs;
    for (int i = 0; i<rvNames.size(); i++) {
        QJsonObject curRV;
        curRV["distribution"] = QString("Normal");
        curRV["inputType"] = QString("Parameters");
        curRV["mean"] = rvValues.at(i);
        curRV["name"] = rvNames.at(i);
        curRV["refCount"] = 1;
        curRV["stdDev"] = 1E-05;
        curRV["value"] = QString("RV.RV_column1");
        curRV["variableClass"] = "Uncertain";
        rvs.append(curRV);
    }
    configObject["randomVariables"] = rvs;

    configObject["localAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
    configObject["remoteAppDir"]=SimCenterPreferences::getInstance()->getAppDir();
    configObject["workingDir"]=SimCenterPreferences::getInstance()->getLocalWorkDir();
    configObject["runType"]=QString("runningLocal");

    QString tmpDirName = QString("tmp.SimCenter");
    QDir workDir(SimCenterPreferences::getInstance()->getLocalWorkDir());
    QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
    configObject["runDir"] = tmpDirectory;

    QDir destinationDirectory(tmpDirectory);
    if(destinationDirectory.exists()) {
      destinationDirectory.removeRecursively();
    } else
      destinationDirectory.mkpath(tmpDirectory);

    /***
    for (int i = 0; i <theRandomVariables.size(); ++i) {

        if (theRandomVariables.at(i)->getAbbreviatedName()!="UserDefVec"){
            QJsonObject rv;
            if (theRandomVariables.at(i)->outputToJSON(rv)) {
                rvArray.append(rv);
            } else {
                qDebug() << "OUTPUT FAILED" << theRandomVariables.at(i)->variableName->text();
                result = false;
            }
        } else {
            //rename X to X_1,X_2,...,X_N

            //int leng = theRandomVariables.at(i)->getLength();
            QString vectorName=theRandomVariables.at(i)->variableName->text();
            QString componentName = vectorName + "_1";
            theRandomVariables.at(i)->variableName->setText(componentName);
            QJsonObject rv;
            if (theRandomVariables.at(i)->outputToJSON(rv)) {
                rv["vectorName"]=vectorName;
                rvArray.append(rv);
            } else {
                qDebug() << "OUTPUT FAILED" << theRandomVariables.at(i)->variableName->text();
                result = false;
            }

            int leng = rv["length"].toInt();
            for (int j =1; j<leng ; j++) {
                QString componentName = vectorName + "_" +QString::number(j+1);
                theRandomVariables.at(i)->variableName->setText(componentName);

                QJsonObject rv;
                if (theRandomVariables.at(i)->outputToJSON(rv)) {
                    rv["vectorName"]=vectorName;
                    rvArray.append(rv);
                } else {
                    qDebug() << "OUTPUT FAILED" << theRandomVariables.at(i)->variableName->text();
                    result = false;
                }
            }
            theRandomVariables.at(i)->variableName->setText(vectorName);

        }

    }

    rvObject["randomVariables"]=rvArray;

    if (correlationMatrix != NULL) {

      qDebug() << "WRITING CORRELATION MATRIX";

        QJsonArray correlationData;
        for (int i = 0; i <theRandomVariables.size(); ++i) {
            for (int j = 0; j <theRandomVariables.size(); ++j) {

                QTableWidgetItem *cellItemFromTable=correlationMatrix->item(i,j);
                double value = 0.0;
                if (cellItemFromTable!=NULL) {
                    value=((cellItemFromTable->text()).toDouble());
        } else {
          value = 0.;
        }
        correlationData.append(value);
            }
        }
        rvObject["correlationMatrix"]=correlationData;
    }
    ***/

    return result;
}

void SpectrumFromRegionalSurrogate::showRVs(QStringList &varNamesAndValues) {

    // show RVs
    int numVar = varNamesAndValues.count();
    //qDebug() << "numVar=" << numVar;
    for (int i = 0; i<numVar; i+= 2) {
        //qDebug() << "i=" << i;
        QString varName = varNamesAndValues.at(i);
        //qDebug() << "varName=" << varName;
        QString value = varNamesAndValues.at(i+1);
        //qDebug() << "value=" << value;
        rvNames.append(varName);
        //qDebug() << "rvNames=" << rvNames;
        rvValues.append(value.toDouble());
        //qDebug() << "rvValues=" << rvValues;
        QHBoxLayout *curLayout = new QHBoxLayout();
        //qDebug() << "curLayout";
        QLabel *curLabel = new QLabel(varName);
        //qDebug() << "curLabel=" << curLabel;
        QLineEdit *curLE = new QLineEdit(value);
        //qDebug() << "curLE";
        rvValueLE.append(curLE);
        curLayout->addWidget(curLabel);
        curLayout->addWidget(curLE);
        rvLayout->addLayout(curLayout);
    }

}

void SpectrumFromRegionalSurrogate::getSpectrum(void) {

    // conducting the surrogate prediction
    bool runFlag = true;

    // refresh random variables
    for (int i = 0; i<rvNames.size(); i++) {
        rvValues[i] = rvValueLE.at(i)->text().toDouble();
    }

    // write json
    QJsonObject configJSON;
    this->writeConfigJSON(configJSON);

    QString templateDir("templatedir");
    QDir destinationDirectory(configJSON.value("runDir").toString());
    QString templateDirectory  = destinationDirectory.absoluteFilePath(templateDir);
    destinationDirectory.mkpath(templateDirectory);
    QString inputFile = templateDirectory + QDir::separator() + tr("scInput.json");

    qDebug() << "INPUT FILE: " << inputFile;
    QFile file(inputFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }
    /* REMOVING CRAP
    QJsonObject json;
    if (this->outputToJSON(json) == false) {
        //this->errorMessage("WorkflowApp - failed in outputToJson");
        return;
    }
    */
    
    QJsonDocument doc(configJSON);
    file.write(doc.toJson());
    file.close();
    qDebug() << "JSON: " << configJSON;

    // copy files
    this->copyFiles(templateDirectory);

    // run qWHALE
    QString tempDirectory = templateDirectory;
    QString inputFilePath = inputFile;

    // qDebug() << "RUNTYPE" << runType;
    QString runType("runningLocal");
    qDebug() << "RUNTYPE" << runType;
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    QString appName = QCoreApplication::applicationName();

    //TODO: recognize if it is PBE or EE-UQ -> probably smarter to do it inside the python file
    QString pySCRIPT;
    QDir scriptDir(appDir);
    scriptDir.cd("applications");
    scriptDir.cd("Workflow");
    pySCRIPT = scriptDir.absoluteFilePath("qWHALE.py"); // invoke qWHALE to run GP surrogate prediction
    QFileInfo check_script(pySCRIPT);
    if (!check_script.exists() || !check_script.isFile()) {
        //emit sendErrorMessage(QString("NO SCRIPT FILE: ") + pySCRIPT);
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }

    QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
    QFileInfo check_registry(registryFile);
    if (!check_registry.exists() || !check_registry.isFile()) {
        //emit sendErrorMessage(QString("NO REGISTRY FILE: ") + registryFile);
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }

    qDebug() << "SCRIPT: " << pySCRIPT;
    qDebug() << "REGISTRY: " << registryFile;

    QStringList files;
    files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";
    qDebug() << "files: " << files;

    //
    // now invoke dakota, done via a python script in tool app dircetory
    //
    proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::SeparateChannels);
    qDebug() << "setProcessChannelMode";
    auto procEnv = QProcessEnvironment::systemEnvironment();
    QString pathEnv = procEnv.value("PATH");
    qDebug() << "pathEnv: " << pathEnv;
    QString pythonPathEnv = procEnv.value("PYTHONPATH");
    qDebug() << "pythonPathEnv: " << pythonPathEnv;
    QString python = QString("python");
    qDebug() << "python: " << python;
    QString exportPath("export PATH=");
    qDebug() << "exportPath: " << exportPath;
    bool colonYes = false;
    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    python = preferences->getPython();
    qDebug() << "python2: " << python;

    QFileInfo pythonFile(python);
    if (pythonFile.exists()) {
        QString pythonPath = pythonFile.absolutePath();
        colonYes=true;
        exportPath += pythonPath;
        pathEnv = pythonPath + ';' + pathEnv;
    } else {
        //emit sendErrorMessage("NO VALID PYTHON - Read the Manual & Check your Preferences");
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }
    QString openseesExe = preferences->getOpenSees();
    QFileInfo openseesFile(openseesExe);
    if (openseesFile.exists()) {
        QString openseesPath = openseesFile.absolutePath();
    qDebug() << "openseesPath: " << openseesPath;

#ifdef Q_OS_WIN
        pathEnv = openseesPath + ';' + pathEnv;
#else
        pathEnv = openseesPath + ':' + pathEnv;
#endif
        if (colonYes == false) {
            colonYes = true;
        } else {
#ifdef Q_OS_WIN
            exportPath += ";";
#else
            exportPath += ":";
#endif
        }
        exportPath += openseesPath;
    }


    QString dakotaExe = preferences->getDakota();

    qDebug() << "DAKOTA: " << dakotaExe;

    QFileInfo dakotaFile(dakotaExe);
    if (dakotaFile.exists()) {
        QString dakotaPath = dakotaFile.absolutePath();
        if (colonYes == false) {
            colonYes = true;
        } else {
#ifdef Q_OS_WIN
            exportPath += ";";
#else
            exportPath += ":";
#endif
        }

#ifdef Q_OS_WIN
        pathEnv = dakotaPath + ';' + pathEnv;
#else
        pathEnv = dakotaPath + ':' + pathEnv;
#endif
        exportPath += dakotaPath;
        QString dakotaPathPath = QFileInfo(dakotaPath).absolutePath() + QDir::separator() +
                "share" + QDir::separator() + "Dakota" + QDir::separator() + "Python";
        pythonPathEnv = dakotaPathPath + ";" + pythonPathEnv;

    }

    if (colonYes == true) {
#ifdef Q_OS_WIN
        exportPath += ";";
#else
        exportPath += ":";
#endif
    }

    exportPath += "$PATH";
    procEnv.insert("PATH", pathEnv);
    procEnv.insert("PYTHONPATH", pythonPathEnv);
    proc->setProcessEnvironment(procEnv);

    QStringList args;
    QString inputDir = tempDirectory + QDir::separator() + "input_data";
    if (appName == "RDT" || appName == "R2D") {
        args << pySCRIPT << inputFile << "--registry" << registryFile
             << "--referenceDir" << inputDir
             << "-w" << tempDirectory+QDir::separator() + "Results";
    } else {
        args << pySCRIPT << runType << inputFile << registryFile;
    }

#ifdef Q_OS_WIN
    python = QString("\"") + python + QString("\"");

    qDebug() << python;
    qDebug() << args;

    proc->start(python,args);

    bool failed = false;
    if (!proc->waitForStarted(-1))
    {
        qDebug() << "Failed to start the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString().split('\n');
        //this->statusMessage("Failed to start the workflow!!!");
        failed = true;
    }

    if(!proc->waitForFinished(-1))
    {
        qDebug() << "Failed to finish running the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        //this->statusMessage("Failed to finish running the workflow!!!");
        failed = true;
    }


    if(0 != proc->exitCode())
    {
        qDebug() << "Failed to run the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        //this->statusMessage("Failed to run the workflow!!!");
        failed = true;
    }

    if(failed)
    {
        qDebug().noquote() << proc->readAllStandardOutput();
        qDebug().noquote() << proc->readAllStandardError();
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }

#else

    // check for bashrc or bash profile
    QDir homeDir(QDir::homePath());
    QString sourceBash("");
    if (homeDir.exists(".bash_profile")) {
        sourceBash = QString("source $HOME/.bash_profile; ");
    } else if (homeDir.exists(".bashrc")) {
        sourceBash = QString("source $HOME/.bashrc; ");
    } else if (homeDir.exists(".zprofile")) {
        sourceBash = QString("source $HOME/.zprofile; ");
    } else if (homeDir.exists(".zshrc")) {
        sourceBash = QString("source $HOME/.zshrc; ");
    } else


      qDebug() << QString("No .bash_profile, .bashrc, .zprofile or .zshrc file found. This may not find Dakota or OpenSees");

    // note the above not working under linux because bash_profile not being called so no env variables!!
    QString command;

    if (appName == "R2D"){

        command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) + pySCRIPT + QString("\" " )
                + QString(" \"" ) + inputFile + QString("\" ") +"--registry"
                + QString(" \"") + registryFile + QString("\" ") + "--referenceDir" + QString(" \"")
                + tempDirectory + QString("/input_data\" ") + "-w" + QString(" \"") + tempDirectory + QDir::separator() + "Results" + QString("\"");

    } else {

        command = sourceBash + exportPath + "; \"" + python + QString("\" \"" ) +
                pySCRIPT + QString("\" " ) + runType + QString(" \"" ) + inputFile + QString("\" \"") + registryFile + QString("\"");

    }
    qDebug() << "PYTHON COMMAND" << command;

    proc->start("bash", QStringList() << "-c" <<  command);
    proc->waitForStarted();

    bool failed = false;
    if(!proc->waitForFinished(-1))
    {
        qDebug() << "Failed to finish running the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
	//        this->statusMessage("Failed to finish running the workflow!!!");
        failed = true;
    }


    if(0 != proc->exitCode())
    {
        qDebug() << "Failed to run the workflow!!! exit code returned: " << proc->exitCode();
        qDebug() << proc->errorString();
        // this->statusMessage("Failed to run the workflow!!!");
        failed = true;
    }

    if(failed)
    {
        qDebug().noquote() << proc->readAllStandardOutput();
        qDebug().noquote() << proc->readAllStandardError();
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }

#endif

    // return
    emit runComplete(true, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
    qDebug() << "runComplete with success.";
}

QString SpectrumFromRegionalSurrogate::checkPeriodsValid(const QString& input) const
{

    QString validInput = input;

    if(validInput.isEmpty())
        return validInput;

    int pos = 0;
    if(LEValidator->validate(const_cast<QString&>(input), pos) != 1)
    {
        validInput = input.left(pos - 1);

        qDebug()<<"pos"<<pos<<" : "<<validInput;
        periodsLineEdit->setText(validInput);
    }

    return validInput;
}

void SpectrumFromRegionalSurrogate::commitPeriods()
{
    auto LEtext = periodsLineEdit->text();
    periodArray = {};

    if(LEtext.isEmpty())
        return;

    auto inputStr = this->checkPeriodsValid(LEtext);

    // Remove any white space from the string
    inputStr.remove(" ");

    // Split the incoming text into the parts delimited by commas
    std::vector<std::string> subStringVec;

    // Create string stream from the string
    std::stringstream s_stream(inputStr.toStdString());

    // Split the input string to substrings at the comma
    while(s_stream.good())
    {
        std:: string subString;
        getline(s_stream, subString, ',');

        if(subString.empty())
            continue;

        subStringVec.push_back(subString);
    }

    for(auto&& subStr : subStringVec)
    {

        // Convert strings into numbers
        auto period = std::stod(subStr);

        periodArray.push_back(period);
    }
}

bool SpectrumFromRegionalSurrogate::copyFiles(QString dirName) {

     QString fileName = inputScript->text();

     if (fileName.isEmpty()) {
         //this->errorMessage("surrogateGPInput - no file set");
         return false;
     }
     QFileInfo fileInfo(fileName);

     QString theFile = fileInfo.fileName();
     QString thePath = fileInfo.path();

     this->copyPath(thePath, dirName, false);

     QStringList varNames = varNamesAndValues;

     // now create special copy of original main script that handles the RV
     surrogateGpParser theParser;
     QString copiedFile = dirName + QDir::separator() + theFile;
     theParser.writeFile(fileName, copiedFile, varNames);

     return true;
}


bool SpectrumFromRegionalSurrogate::copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
  // don't copy self!
  if (sourceDir == destinationDir)
    return true;

  QDir sourceDirectory(sourceDir);

  if (! sourceDirectory.exists()) {
      qDebug() << "Source Directory: " << sourceDir << " Does not exist";
      return false;
  }

  QDir destinationDirectory(destinationDir);

  if(destinationDirectory.exists() && overWriteDirectory) {
      destinationDirectory.removeRecursively();
  }

  foreach (QString directoryName, sourceDirectory.entryList(QDir::Dirs | \
                                                            QDir::NoDotAndDotDot))
  {
      if (directoryName != QString("tmp.SimCenter")) {
          QString destinationPath = destinationDir + "/" + directoryName;
          sourceDirectory.mkpath(destinationPath);
          copyPath(sourceDir + "/" + directoryName, destinationPath, overWriteDirectory);
      }
  }

  foreach (QString fileName, sourceDirectory.entryList(QDir::Files)) {
      QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);
  }

  /*! Possible race-condition mitigation? */

  QDir finalDestination(destinationDir);
  finalDestination.refresh();

  if(finalDestination.exists()) {
      return true;
  }
  return false;
}

void SpectrumFromRegionalSurrogate::saveSpectrum(bool runFlag, QString dirName, QString fileName) {
    qDebug() << "runFlag: " << runFlag;
    if (runFlag) {
        // first load the dakota.csv (spectrum)
        QString filenameOUT = dirName + QDir::separator() + tr("dakota.out");
        QString filenameTAB = dirName + QDir::separator() + tr("dakotaTab.out");

        qDebug() << "filenameTAB: " << filenameTAB;
        qDebug() << "filenameOUT: " << filenameOUT;

        QFileInfo fileTabInfo(filenameTAB);
        QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");

        QFileInfo filenameErrorInfo(filenameErrorString);
        if (!filenameErrorInfo.exists()) {
            //errorMessage("No error file - SimCenterUQ did not run - problem with SimCenterUQ setup or the applications failed with inputs provided");
            qDebug() << "No error file - SimCenterUQ did not run - problem with SimCenterUQ setup or the applications failed with inputs provided";
            return;
        }

        QFile fileError(filenameErrorString);
        QString line("");
        if (fileError.open(QIODevice::ReadOnly)) {
            QTextStream in(&fileError);
            QString contents = in.readAll();
            if (!contents.isEmpty()) {
                //errorMessage(QString(QString("Error Running SimCenterUQ: ") + contents));
                qDebug() << "Error Running SimCenterUQ: "+ contents;
                return;
            }
        }

        QFileInfo filenameTabInfo(filenameTAB);
        if (!filenameTabInfo.exists()) {
            //errorMessage("No SimCenterUQTab.out file - SimCenterUQ failed .. possibly no QoI");
            qDebug() << "No SimCenterUQTab.out file - SimCenterUQ failed .. possibly no QoI";
            return;
        }

        // read csv
        QStringList theHeadings;
        QStringList res_data;
        theHeadings << "Run #";
        std::ifstream tabResults(filenameTAB.toStdString().c_str());
        if (!tabResults.is_open()) {
            qDebug() << "Could not open file";
            return;
        }
        std::string inputLine;
        std::getline(tabResults, inputLine);
        std::istringstream iss(inputLine);
        int colCount = 0;
        bool includesInterface = false;
        do {
            std::string subs;
            iss >> subs;
            if (colCount > 0) {
                if (subs != " ") {
                    if (subs != "interface")
                        theHeadings << subs.c_str();
                    else
                        includesInterface = true;
                }
            }
            colCount++;
        } while (iss);

        if (includesInterface == true)
            colCount = colCount-2;
        else
            colCount = colCount -1;

        int rowCount = 0;
        while (std::getline(tabResults, inputLine)) {
            std::istringstream is(inputLine);
            int col=0;
            for (int i=0; i<colCount+2; i++) {
                std::string data;
                is >> data;
                if (i == 0) {
                    col++;
                } else if ((includesInterface == true && i !=1) || (includesInterface == false)) {
                    res_data.append(data.c_str());
                    col++;
                }
            }
            rowCount++;
        }

        // write a csv file
        QFile csv_file(dirName + QDir::separator() + fileName);
        qDebug() << "csv_file: " << csv_file.fileName();
        csv_file.open(QIODevice::Append | QIODevice::Text);
        QTextStream stream(&csv_file);
        QString separator(",");
        stream << "T (sec)" + separator + "Sa (g)" + separator << "\n";
        qDebug() << "rvNames.size(): " << rvNames.size() << "\n";
        for (int i = rvNames.size(); i<rvNames.size()+periodArray.size(); i++) {
            stream << periodArray.at(i-rvNames.size()) << separator << res_data.at(i).toDouble() << separator << "\n";
        }
        stream.flush();
        csv_file.close();
    } else {
        return;
    }

    emit spectrumSaved(dirName, fileName);

}
