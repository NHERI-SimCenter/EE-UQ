#include "NoSpectrumUniform.h"
#include "SimCenterPreferences.h"
#include "SimCenterHtmlWidget.h"
#include "ModularPython.h"
#include <QGridLayout>
#include <QLabel>
#include <QDoubleValidator>
#include <QDir>
#include <QJsonDocument>
#include <QProcess>
#include <QJsonObject>
#include <QJsonArray>

NoSpectrumUniform::NoSpectrumUniform(QWidget *parent)  : SimCenterAppWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto positiveDoubleValidator = new QDoubleValidator();
    positiveDoubleValidator->setBottom(0.0);

    numSampPerBin = new QLineEdit("4");
    theSCIMWidget_grid = new SimCenterIntensityMeasureWidget();
    theSCIMWidget_grid->addGridField();
    numTotalSamp = new QLabel("Total number of ground motions is 0.");

    layout->addWidget(new QLabel("# samples per bin"), 0, 0);
    layout->addWidget(numSampPerBin, 0, 1);
    layout->addWidget(theSCIMWidget_grid, 1, 0,1,-1);
    layout->addWidget(numTotalSamp, 2, 0,1,-1);

    layout->setColumnStretch(2,1);

    connect(theSCIMWidget_grid, SIGNAL(numBinsChanged(int)), this, SLOT(updateNumTotalSamp(int)));

}

//QJsonObject NoSpectrumUniform::serialize() const
//{
//    QJsonObject json;
//    json["numSampPerBin"] = numSampPerBin->text();
//    theSCIMWidget_grid->outputToJSON(json);
//    return json;
//}

/*oid NoSpectrumUniform::deserialize(const QJsonObject &json)
{
    QJsonObject* myNonRefJson = const_cast<QJsonObject*>(&json);

    numSampPerBin->setText(json["numSampPerBin"].toString());
    theSCIMWidget_grid->inputFromJSON(*myNonRefJson);
}*/

//QList<QPair<double, double>> NoSpectrumUniform::spectrum() const
//{
//    QList<QPair<double, double>>  targetSectrum;
//    return targetSectrum;
//}

void NoSpectrumUniform::updateNumTotalSamp(int numBins) {
    int num = numSampPerBin->text().toInt() * numBins;
    numTotalSamp -> setText("The number of ground motions to be selected is " + QString::number(num) + ".");
}


bool NoSpectrumUniform::writeConfigJSON(QJsonObject &myJson) {
    myJson["numSampPerBin"]=numSampPerBin->text();
    QJsonObject imJson;
    auto res = theSCIMWidget_grid->outputToJSON(imJson);
    myJson.insert("IM", imJson);
    return res;
}

void NoSpectrumUniform::getRSN(QString workDirPath, QStringList &RSN, QVector<double> &additionalScaling, QString &imagePath) {

    RSN = QStringList({""});

    QJsonObject configJSON; // Input information

    // clean up working directory
    //QDir workDir(SimCenterPreferences::getInstance()->getLocalWorkDir());
    //QDir workDir(workDirPath);
    //QString tmpDirName("tmp.SimCenter");
    //QString templateDir("templatedir");
    //QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
    QDir workDir(workDirPath);

    if(workDir.exists()) {
      workDir.remove("gridIM_input.json");
      workDir.remove("gridIM_output.json");
      workDir.remove("gridIM_log.err");
      workDir.remove("gridIM_coverage.html");
    }


    // write json
    bool res = this->writeConfigJSON(configJSON);

    if (res==false) {
        return;
    }
    // important
    QString inputFilePath = workDirPath + QDir::separator() + tr("gridIM_input.json");
    QString outputFilePath = workDirPath + QDir::separator() + tr("gridIM_output.json");
    QString errFilePath = workDirPath + QDir::separator() + tr("gridIM_log.err");
    imagePath = workDirPath + QDir::separator() + tr("gridIM_coverage.html");

    qDebug() << "INPUT FILE: " << inputFilePath;
    QFile file(inputFilePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QJsonDocument doc(configJSON);
    file.write(doc.toJson());
    file.close();
    qDebug() << "JSON: " << configJSON;

    // run myScript
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    qDebug() << "appDir: " << appDir;

    QDir scriptDir(appDir +QDir::separator());
    scriptDir.cd("applications");
    scriptDir.cd("Workflow");

    QString registryFilePath = scriptDir.absoluteFilePath("WorkflowApplications.json");
    qDebug() << "REGISTRY: " << registryFilePath;
    QFileInfo check_registry(registryFilePath);
    if (!check_registry.exists() || !check_registry.isFile()) {
        errorMessage(QString("Registry file does not exist. Reset the preference or set correct backend applications folder."));
        return ;
    }

    QFile registryFile(registryFilePath);
    if (!registryFile.open(QFile::ReadOnly | QFile::Text)) {
        errorMessage(QString("Registry file cannot be opened."));
        return;
    }
    //
    // place contents of file into json object
    //

    QByteArray tmp =  registryFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(tmp);
    QJsonObject jsonObj = jsonDoc.object();

    QJsonObject EVT_Apps1 = jsonObj["EventApplications"].toObject();
    QJsonArray EVT_Apps2 = EVT_Apps1["Applications"].toArray();


    QString GmSelectionScriptPath = ""; // python file
    for(const QJsonValue& val: EVT_Apps2) {
        QJsonObject loopObj = val.toObject();
        if (loopObj["Name"].toString() == "gridGroundMoionSelection") {
            GmSelectionScriptPath = appDir + QDir::separator() + loopObj["ExecutablePath"].toString();
        }
    }


     QFileInfo check_selectionScript(GmSelectionScriptPath);

     if (!check_selectionScript.exists() || !check_selectionScript.isFile()) {
         errorMessage(QString("Failed find ")+GmSelectionScriptPath);
         return;
     }


    QStringList files;
    //files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";
    files << "gridIM_input.json";
    qDebug() << "files: " << files;
    //
    // now invoke dakota, done via a python script in tool app dircetory
    //

    ModularPython *thePy = new ModularPython(workDirPath);
    thePy->run(GmSelectionScriptPath,files);
/*
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
        emit runComplete(false, configJSON.value("runDir").toString(), "gridIM_output.json");
        return;
    }

    exportPath += "$PATH";
    procEnv.insert("PATH", pathEnv);
    procEnv.insert("PYTHONPATH", pythonPathEnv);
    proc->setProcessEnvironment(procEnv);

    QStringList args;
    args << GmSelectionScriptPath << files;

#ifdef Q_OS_WIN
    python = QString("\"") + python + QString("\"");

    qDebug() << python;
    qDebug() << args;

    proc->setWorkingDirectory(workDirPath);
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


    if((0 != proc->exitCode())&&(-1 != proc->exitCode())) // latter is caputred error
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
        emit runComplete(false, configJSON.value("runDir").toString(), "gridIM_output.json");
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

        command = "\"" + python + QString("\" \"" ) +
                inputFile + QString("\" \"") + registryFile + QString("\"");

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
        emit runComplete(false, configJSON.value("runDir").toString(), "gridIM_output.json");
        return;
    }

#endif
*/
    qDebug() << "Output File Path" << outputFilePath;
    QFile resultsFile(outputFilePath);
    if (!resultsFile.open(QFile::ReadOnly | QFile::Text)) {
        QFile errFile(errFilePath);

        QString line("");
        if (errFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&errFile);
            while (!in.atEnd()) {
                line = in.readLine();
            }
            errFile.close();
            errorMessage("RECORD SELECTION FAILED:" + line);
            return;
        } else {
            return;
        }

        return;
    }

    //
    // place contents of file into json object
    //

    QJsonDocument jsonOutDoc = QJsonDocument::fromJson(resultsFile.readAll());
    QJsonObject jsonOutObj = jsonOutDoc.object();

    //
    // Read GM RSN
    //
    QJsonArray RSN_array = jsonOutObj["gm_RSN"].toArray();
    for (int i=0; i<RSN_array.size();i++) {
        RSN << QString::number(RSN_array[i].toDouble());
    }

    //
    // Read GM custom scale
    //
    QJsonArray Scale_array = jsonOutObj["gm_scale"].toArray();

    for (int i=0; i<Scale_array.size();i++) {
        additionalScaling.push_back(Scale_array[i].toDouble());
    }

    // return
    //emit runComplete(true, configJSON.value("runDir").toString(), "gridIM_output.json");
    qDebug() << "runComplete with success.";

}


