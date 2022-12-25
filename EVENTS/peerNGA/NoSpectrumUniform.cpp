#include "NoSpectrumUniform.h"
#include "SimCenterPreferences.h"
#include <QGridLayout>
#include <QLabel>
#include <QDoubleValidator>
#include <QDir>
#include <QJsonDocument>
#include <QProcess>

NoSpectrumUniform::NoSpectrumUniform(QWidget *parent) : AbstractTargetWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto positiveDoubleValidator = new QDoubleValidator();
    positiveDoubleValidator->setBottom(0.0);

    numSampPerBin = new QLineEdit("4");
    theSCIMWidget_grid = new SimCenterIntensityMeasureWidget();
    theSCIMWidget_grid->addGridField();

    layout->addWidget(new QLabel("# samples per bin"), 0, 0);
    layout->addWidget(numSampPerBin, 0, 1);
    layout->addWidget(theSCIMWidget_grid, 1, 0,1,-1);
    layout->setColumnStretch(2,1);
}


QJsonObject NoSpectrumUniform::serialize() const
{
    QJsonObject json;
    json["numSampPerBin"] = numSampPerBin->text();
    theSCIMWidget_grid->outputToJSON(json);
    return json;
}

void NoSpectrumUniform::deserialize(const QJsonObject &json)
{
    QJsonObject* myNonRefJson = const_cast<QJsonObject*>(&json);

    numSampPerBin->setText(json["numSampPerBin"].toString());
    theSCIMWidget_grid->inputFromJSON(*myNonRefJson);
}

QList<QPair<double, double>> NoSpectrumUniform::spectrum() const
{
    QList<QPair<double, double>>  targetSectrum;
//    QList<double> periods;
//    // Use a 0.05 s interval (0.05s ~ 10s)
//    for (int i = 0; i < 200; ++i)
//        periods << 0.05 * i + 0.05;

//    // Compute ASCE7-10 two-period spectrum
//    auto Sds = sdsEditBox->text().toDouble();
//    auto Sd1 = sd1EditBox->text().toDouble();
//    auto Tl = tlEditBox->text().toDouble();
//    auto Ts =  Sd1 / Sds;
//    auto T0 = 0.2 * Ts;
//    for (int i = 0; i != periods.size()-1; ++i)
//    {
//        if (periods[i] <= T0)
//            targetSectrum.append({periods[i], Sds*(0.4+0.6*periods[i]/T0)});
//        else if (periods[i] <= Ts)
//            targetSectrum.append({periods[i], Sds});
//        else if (periods[i] <= Tl)
//            targetSectrum.append({periods[i], Sd1/periods[i]});
//        else
//            targetSectrum.append({periods[i], Sd1*Tl/periods[i]/periods[i]});
//    }

    return targetSectrum;
}


void NoSpectrumUniform::writeConfigJSON(QJsonObject myJson) {


}

void NoSpectrumUniform::getUniformRSN(void) {


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

    // run myScript
    QString tempDirectory = templateDirectory;
    QString inputFilePath = inputFile;

    // qDebug() << "RUNTYPE" << runType;
    //QString runType("runningLocal");
    //qDebug() << "RUNTYPE" << runType;
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();
    //QString appName = QCoreApplication::applicationName();

//    //TODO: recognize if it is PBE or EE-UQ -> probably smarter to do it inside the python file
//    QString pySCRIPT;
    QDir scriptDir(appDir);
//    //scriptDir.cd("applications");
//    //scriptDir.cd("Workflow");
//    //pySCRIPT = scriptDir.absoluteFilePath("qWHALE.py"); // invoke qWHALE to run GP surrogate prediction
//    pySCRIPT = "Path/To/GMselector";
//    QFileInfo check_script(pySCRIPT);
//    if (!check_script.exists() || !check_script.isFile()) {
//        //emit sendErrorMessage(QString("NO SCRIPT FILE: ") + pySCRIPT);
//        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
//        return;
//    }

    QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
    QFileInfo check_registry(registryFile);
    if (!check_registry.exists() || !check_registry.isFile()) {
        //emit sendErrorMessage(QString("NO REGISTRY FILE: ") + registryFile);
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }

//    qDebug() << "SCRIPT: " << pySCRIPT;
    qDebug() << "REGISTRY: " << registryFile;

    QStringList files;
    //files << "dakota.in" << "dakota.out" << "dakotaTab.out" << "dakota.err";
    //qDebug() << "files: " << files;
    files << "RSN.out";
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

//    exportPath += "$PATH";
//    procEnv.insert("PATH", pathEnv);
//    procEnv.insert("PYTHONPATH", pythonPathEnv);
//    proc->setProcessEnvironment(procEnv);

    QStringList args;
        args << inputFile << registryFile;

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
        emit runComplete(false, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
        return;
    }

#endif

    // return
    emit runComplete(true, configJSON.value("runDir").toString(), "surrogateSpectrum.csv");
    qDebug() << "runComplete with success.";
}


