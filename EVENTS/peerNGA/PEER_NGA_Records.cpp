#include "PEER_NGA_Records.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include "PeerLoginDialog.h"
#include "ZipUtils.h"
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include<QJsonObject>
#include<QJsonArray>
#include <QPair>
#include <SimCenterGraphPlot.h>
#include <QApplication>
#include <QStatusBar>
#include <QMainWindow>
#include <QThread>
#include "ASCE710Target.h"
#include "UserSpectrumWidget.h"
#include "USGSTargetWidget.h"
#include "NSHMPTarget.h"
#include <GoogleAnalytics.h>

PEER_NGA_Records::PEER_NGA_Records(GeneralInformationWidget* generalInfoWidget, QWidget *parent) : SimCenterAppWidget(parent), groundMotionsFolder(QDir::tempPath())
{
    setupUI(generalInfoWidget);

    setupConnections();
}

void PEER_NGA_Records::setupUI(GeneralInformationWidget* generalInfoWidget)
{
    auto layout = new QGridLayout(this);

    auto positiveIntegerValidator = new QIntValidator();
    positiveIntegerValidator->setBottom(1);

    auto positiveDoubleValidator = new QDoubleValidator();
    positiveDoubleValidator->setBottom(0.0);

    auto targetSpectrumGroup = new QGroupBox("Target Spectrum");

    auto targetSpectrumLayout = new QGridLayout(targetSpectrumGroup);
    targetSpectrumLayout->setColumnMinimumWidth(1, 100);
    targetSpectrumLayout->setColumnMinimumWidth(2, 30);

    targetSpectrumLayout->addWidget(new QLabel("Type"), 0, 0);
    spectrumTypeComboBox = new QComboBox();

    targetSpectrumLayout->addWidget(spectrumTypeComboBox, 0, 1);
    spectrumTypeComboBox->addItem("Design Spectrum (ASCE 7-10)");
    spectrumTypeComboBox->addItem("User Specified");
    spectrumTypeComboBox->addItem("Design Spectrum (USGS Web Service)");
    spectrumTypeComboBox->addItem("Uniform Hazard Spectrum (USGS NSHMP)");

    targetSpectrumDetails = new QStackedWidget(this);
    targetSpectrumLayout->addWidget(targetSpectrumDetails, 1, 0, 1, 3);
    auto asce710Target = new ASCE710Target(this);
    targetSpectrumDetails->addWidget(asce710Target);
    auto userSpectrumTarget = new UserSpectrumWidget(this);
    targetSpectrumDetails->addWidget(userSpectrumTarget);
    auto usgsSpectrumTarget = new USGSTargetWidget(generalInfoWidget, this);
    targetSpectrumDetails->addWidget(usgsSpectrumTarget);
    auto nshmpTarget = new NSHMPTarget(generalInfoWidget, this);
    targetSpectrumDetails->addWidget(nshmpTarget);

    auto recordSelectionGroup = new QGroupBox("Record Selection");
    recordSelectionLayout = new QGridLayout(recordSelectionGroup);
    recordSelectionLayout->addWidget(new QLabel("Number of Records"), 0, 0);
    nRecordsEditBox = new QLineEdit("16");
    nRecordsEditBox->setValidator(positiveIntegerValidator);
    recordSelectionLayout->addWidget(nRecordsEditBox, 0, 1);

    //Magnitude Range
    magnitudeCheckBox = new QCheckBox("Magnitude");
    recordSelectionLayout->addWidget(magnitudeCheckBox, 1, 0);
    magnitudeMin = new QLineEdit("5.0");
    magnitudeMin->setEnabled(false);
    magnitudeMin->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(magnitudeMin, 1, 1);
    magnitudeMax = new QLineEdit("8.0");
    magnitudeMax->setEnabled(false);
    magnitudeMax->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(magnitudeMax, 1, 2);

    distanceCheckBox = new QCheckBox("Distance");
    recordSelectionLayout->addWidget(distanceCheckBox, 2, 0);
    distanceMin = new QLineEdit("0.0");
    distanceMin->setEnabled(false);
    distanceMin->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(distanceMin, 2, 1);
    distanceMax = new QLineEdit("50.0");
    distanceMax->setEnabled(false);
    distanceMax->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(distanceMax, 2, 2);
    recordSelectionLayout->addWidget(new QLabel("km"), 2, 3);

    vs30CheckBox = new QCheckBox("Vs30");
    recordSelectionLayout->addWidget(vs30CheckBox, 3, 0);
    vs30Min = new QLineEdit("150.0");
    vs30Min->setEnabled(false);
    vs30Min->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(vs30Min, 3, 1);
    vs30Max = new QLineEdit("300.0");
    vs30Max->setEnabled(false);
    vs30Max->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(vs30Max, 3, 2);
    recordSelectionLayout->addWidget(new QLabel("m/s"), 3, 3);

    targetSpectrumGroup->setMaximumHeight(200);
    recordSelectionGroup->setMaximumHeight(200);

    auto scalingGroup = new QGroupBox("Scaling");
    auto scalingLayout = new QGridLayout(scalingGroup);

    scalingComboBox = new QComboBox();
    scalingComboBox->addItem("No Scaling");
    scalingComboBox->addItem("Minimize MSE");
    scalingComboBox->addItem("Single Period");

    scalingLayout->addWidget(new QLabel("Scaling Method:"), 0, 0);
    scalingLayout->addWidget(scalingComboBox, 0, 1);

    scalingPeriodLabel1 = new QLabel("Scaling Period (sec):");
    scalingPeriodLineEdit = new QLineEdit("1.0");
    scalingPeriodLabel2  = new QLabel("(Ti)");
    scalingLayout->addWidget(scalingPeriodLabel1, 0, 2);
    scalingLayout->addWidget(scalingPeriodLineEdit, 0, 3);
    scalingLayout->addWidget(scalingPeriodLabel2, 0, 4);

    weightFunctionHeadingLabel = new QLabel("Weight Function");
    weightFunctionHeadingLabel->setStyleSheet("font-weight: bold;");

    weightFunctionLabel = new QLabel("Weight function is used in both search and scaling when computing MSE. Values can be updated for rescaling. Intermediate points are interpolated with W = fxn(log(T))");
    weightFunctionLabel->setWordWrap(true);

    periodPointsLabel1 = new QLabel("Period Points :");
    periodPointsLineEdit = new QLineEdit("0.01,0.05,0.1,0.5,1,5,10.0");
    periodPointsLabel2 = new QLabel("(T1,T2, ... Tn)");

    weightsLabel1 = new QLabel("Weights :");
    weightsLineEdit = new QLineEdit("1.0,1.0,1.0,1.0,1.0,1.0,1.0");
    weightsLabel2 = new QLabel("(W1,W2, ... Wn)");

    scalingLayout->addWidget(weightFunctionHeadingLabel, 1, 0);
    scalingLayout->addWidget(weightFunctionLabel, 1, 3, 4, 2);
    scalingLayout->addWidget(periodPointsLabel1, 2, 0);
    scalingLayout->addWidget(periodPointsLineEdit, 2, 1);
    scalingLayout->addWidget(periodPointsLabel2, 2, 2);
    scalingLayout->addWidget(weightsLabel1, 3, 0);
    scalingLayout->addWidget(weightsLineEdit, 3, 1);
    scalingLayout->addWidget(weightsLabel2, 3, 2);

    selectRecordsButton = new QPushButton("Select Records");
    scalingLayout->addWidget(selectRecordsButton, 4, 0, 1, 2);

    this->onScalingComboBoxChanged(0);

    //Records Table
    recordsTable = new QTableWidget();
    recordsTable->setHidden(true);

    //Ground Motions
    auto groundMotionsGroup = new QGroupBox("Ground Motions");
    auto groundMotionsLayout = new QGridLayout(groundMotionsGroup);
    groundMotionsComponentsBox = new QComboBox();
    groundMotionsComponentsBox->addItem("One (Horizontal)", GroundMotionComponents::One);
    groundMotionsComponentsBox->addItem("Two (Horizontal)", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("Three (Horizontal & Vertical)", GroundMotionComponents::Three);
    groundMotionsLayout->addWidget(new QLabel("Acceleration Components"), 0, 0);
    groundMotionsLayout->addWidget(groundMotionsComponentsBox, 0, 1);
    recordsTable->setMinimumHeight(200);
    groundMotionsLayout->addWidget(recordsTable, 1, 0, 1, 2);
    groundMotionsLayout->setRowStretch(1, 1);
    progressBar = new QProgressBar();
    progressBar->setRange(0,0);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setHidden(true);

    groundMotionsLayout->addWidget(progressBar, 2, 0, 1, 2);

    layout->addWidget(targetSpectrumGroup, 0, 0);
    layout->addWidget(recordSelectionGroup, 0, 1);
    layout->addWidget(scalingGroup, 1, 0, 1, 2);
    layout->addWidget(groundMotionsGroup, 2, 0, 1, 2);

    auto peerCitation = new QLabel("This tool uses PEER NGA West 2 Ground Motions Database. "
                                   "Users should cite the database as follows: PEER 2013/03 – PEER NGA-West2 Database, "
                                   "Timothy D. Ancheta, Robert B. Darragh, Jonathan P. Stewart, Emel Seyhan, Walter J. Silva, "
                                   "Brian S.J. Chiou, Katie E. Wooddell, Robert W. Graves, Albert R. Kottke, "
                                   "David M. Boore, Tadahiro Kishida, and Jennifer L. Donahue.");

    peerCitation->setWordWrap(true);
    layout->addWidget(peerCitation, 3, 0, 1, 3);

    //layout->addWidget(thePlottingWindow, 0,3,2,1);
    layout->addWidget(&recordSelectionPlot, 0,3,4,1);

    recordSelectionPlot.setHidden(true);

    layout->setRowStretch(layout->rowCount(), 1);
    layout->setColumnStretch(layout->columnCount(), 1);

}

void PEER_NGA_Records::setupConnections()
{
    connect(selectRecordsButton, &QPushButton::clicked, this, [this]()
    {

        if(!peerClient.loggedIn())
        {
            PeerLoginDialog loginDialog(&peerClient, this);
            loginDialog.setWindowModality(Qt::ApplicationModal);
            loginDialog.exec();
            loginDialog.close();
            if(loginDialog.result() != QDialog::Accepted)
                return;
        }

        selectRecords();
    });

    connect(&peerClient, &PeerNgaWest2Client::recordsDownloaded, this, [this](QString recordsFile)
    {
        auto tempRecordsDir = QDir(groundMotionsFolder.path());
        //Cleaning up previous search results
        if(tempRecordsDir.exists("_SearchResults.csv"))
            tempRecordsDir.remove("_SearchResults.csv");
        ZipUtils::UnzipFile(recordsFile, tempRecordsDir);
        processPeerRecords(tempRecordsDir);
    });

    connect(magnitudeCheckBox, &QCheckBox::clicked, this, [this](bool checked){
        magnitudeMin->setEnabled(checked);
        magnitudeMax->setEnabled(checked);
    });

    connect(distanceCheckBox, &QCheckBox::clicked, this, [this](bool checked){
        distanceMin->setEnabled(checked);
        distanceMax->setEnabled(checked);
    });

    connect(vs30CheckBox, &QCheckBox::clicked, this, [this](bool checked){
        vs30Min->setEnabled(checked);
        vs30Max->setEnabled(checked);
    });

    connect(&peerClient, &PeerNgaWest2Client::statusUpdated, this, &PEER_NGA_Records::updateStatus);

    connect(&peerClient, &PeerNgaWest2Client::selectionStarted, this, [this]()
    {
        this->progressBar->setHidden(false);
        this->selectRecordsButton->setEnabled(false);
        this->selectRecordsButton->setDown(true);
    });



    connect(&peerClient, &PeerNgaWest2Client::selectionFinished, this, [this]()
    {
        this->progressBar->setHidden(true);
        this->selectRecordsButton->setEnabled(true);
        this->selectRecordsButton->setDown(false);
    });

    connect(recordsTable, &QTableWidget::itemSelectionChanged, this, [this]()
    {
        QList<int> selectedRows;

        auto selectedRanges = recordsTable->selectedRanges();
        for (auto range: selectedRanges)
        {
            for (int i = range.topRow(); i <= range.bottomRow(); i++)
                selectedRows << i;
        }

        recordSelectionPlot.highlightSpectra(selectedRows);
    });

    connect(spectrumTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        targetSpectrumDetails->setCurrentIndex(index);

        return;
    });


    for(int i = 0; i < targetSpectrumDetails->count(); i++)
    {
        auto targetWidget = reinterpret_cast<AbstractTargetWidget*>(targetSpectrumDetails->widget(i));
        connect(targetWidget, &AbstractTargetWidget::statusUpdated, this, &PEER_NGA_Records::updateStatus);
    }


    connect(scalingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onScalingComboBoxChanged(int)));
}

void PEER_NGA_Records::processPeerRecords(QDir resultFolder)
{
    emit sendStatusMessage(QString("Parsing Downloaded Records"));
    if(!resultFolder.exists())
        return;

    clearSpectra();

    currentRecords = parseSearchResults(resultFolder.filePath("_SearchResults.csv"));
    setRecordsTable(currentRecords);

    plotSpectra();
    emit sendStatusMessage(QString(""));
}

void PEER_NGA_Records::setRecordsTable(QList<PeerScaledRecord> records)
{
    recordsTable->clear();
    int row = 0;
    recordsTable->setRowCount(records.size());
    recordsTable->setColumnCount(10);
    recordsTable->setHorizontalHeaderLabels(QStringList({"RSN","Scale", "Earthquake", "Station",
                                                         "Magnitude", "Distance", "Vs30",
                                                         "Horizontal 1 File", "Horizontal 2 File", "Vertical File"}));
    for(auto& record: records)
    {
        addTableItem(row, 0, QString::number(record.RSN));
        addTableItem(row, 1, QString::number(record.Scale));
        addTableItem(row, 2, record.Earthquake);
        addTableItem(row, 3, record.Station);
        addTableItem(row, 4, QString::number(record.Magnitude));
        addTableItem(row, 5, QString::number(record.Distance));
        addTableItem(row, 6, QString::number(record.Vs30));
        addTableItem(row, 7, record.Horizontal1File);
        addTableItem(row, 8, record.Horizontal2File);
        addTableItem(row, 9, record.VerticalFile);
        row++;
    }
    recordsTable->resizeColumnsToContents();
    recordsTable->setHidden(false);

}

void PEER_NGA_Records::clearSpectra()
{
    periods.clear();
    meanSpectrum.clear();
    meanPlusSigmaSpectrum.clear();
    meanMinusSigmaSpectrum.clear();
    targetSpectrum.clear();
    scaledSelectedSpectra.clear();
}

void PEER_NGA_Records::plotSpectra()
{
    //Spectra can be plotted here using the data in
    //periods, targetSpectrum, meanSpectrum, meanPlusSigmaSpectrum, meanMinusSigmaSpectrum, scaledSelectedSpectra

    recordSelectionPlot.setHidden(false);
    recordSelectionPlot.setSelectedSpectra(periods, scaledSelectedSpectra);
    recordSelectionPlot.setMean(periods, meanSpectrum);
    recordSelectionPlot.setMeanPlusSigma(periods, meanPlusSigmaSpectrum);
    recordSelectionPlot.setMeanMinusSigma(periods, meanMinusSigmaSpectrum);
    recordSelectionPlot.setTargetSpectrum(periods, targetSpectrum);

    auto size = recordSelectionPlot.size();
    size.setWidth(size.height());
    recordSelectionPlot.setMinimumSize(size);
}

void PEER_NGA_Records::updateStatus(QString status)
{
    emit sendStatusMessage(status);

    // Showing status in status bar
    if(this->parent())
    {
        auto topWidget = this->parent();
        while(topWidget->parent()) topWidget = topWidget->parent();

        auto statusBar = static_cast<QMainWindow*>(topWidget)->statusBar();
        if (statusBar)
            statusBar->showMessage(status, 5000);
    }
    return;
}

void PEER_NGA_Records::selectRecords()
{

    // Set the search scaling parameters
    // 0 is not scaling
    // 1 is minimize MSE
    // 2 is single period scaling
    auto scaleFlag = scalingComboBox->currentIndex();
    auto periodPoints = periodPointsLineEdit->text();
    auto weightPoints = weightsLineEdit->text();
    auto scalingPeriod = scalingPeriodLineEdit->text();
    peerClient.setScalingParameters(scaleFlag,periodPoints,weightPoints,scalingPeriod);

    GoogleAnalytics::Report("RecordSelection", "PEER");

    QVariant magnitudeRange;
    if(magnitudeCheckBox->checkState() == Qt::Checked)
        magnitudeRange.setValue(qMakePair(magnitudeMin->text().toDouble(), magnitudeMax->text().toDouble()));

    QVariant distanceRange;
    if(distanceCheckBox->checkState() == Qt::Checked)
        distanceRange.setValue(qMakePair(distanceMin->text().toDouble(), distanceMax->text().toDouble()));

    QVariant vs30Range;
    if(vs30CheckBox->checkState() == Qt::Checked)
        vs30Range.setValue(qMakePair(vs30Min->text().toDouble(), vs30Max->text().toDouble()));

    if(targetSpectrumDetails->currentIndex() == 0)
    {
        auto asce710widget = reinterpret_cast<ASCE710Target*>(targetSpectrumDetails->currentWidget());
        peerClient.selectRecords(asce710widget->sds(),
                                 asce710widget->sd1(),
                                 asce710widget->tl(),
                                 nRecordsEditBox->text().toInt(), magnitudeRange, distanceRange, vs30Range);
    }
    else
    {
        auto userTargetWidget = reinterpret_cast<AbstractTargetWidget*>(targetSpectrumDetails->currentWidget());

        progressBar->setHidden("False");
        selectRecordsButton->setEnabled(false);
        selectRecordsButton->setDown(true);

        updateStatus("Retrieving Target Spectrum...");
        auto spectrum = userTargetWidget->spectrum();


        if (spectrum.size() > 0)
            peerClient.selectRecords(userTargetWidget->spectrum(), nRecordsEditBox->text().toInt(), magnitudeRange, distanceRange, vs30Range);
        else
        {
            progressBar->setHidden("True");
            selectRecordsButton->setEnabled(true);
            selectRecordsButton->setDown(false);
        }
    }
}

void PEER_NGA_Records::addTableItem(int row, int column, QString value)
{
    auto item = new QTableWidgetItem(value);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    recordsTable->setItem(row, column, item);
}

QList<PeerScaledRecord> PEER_NGA_Records::parseSearchResults(QString searchResultsFilePath)
{
    QList<PeerScaledRecord> records;
    records.reserve(nRecordsEditBox->text().toInt());
    QFile searchResultsFile(searchResultsFilePath);
    if(!searchResultsFile.exists())
        return records;

    if(!searchResultsFile.open(QFile::ReadOnly))
        return records;

    QTextStream searchResultsStream(&searchResultsFile);
    while (!searchResultsStream.atEnd())
    {
        QString line = searchResultsStream.readLine();

        //Parsing selected records information
        if(line.contains("Metadata of Selected Records"))
        {
            //skip header
            searchResultsStream.readLine();
            line = searchResultsStream.readLine();

            while(!line.isEmpty())
            {
                auto values = line.split(',');
                PeerScaledRecord record;
                record.RSN = values[2].toInt();
                record.Earthquake = values[9].trimmed().remove('\"');
                record.Station = values[11].trimmed().remove('\"');
                record.Magnitude = values[12].trimmed().toDouble();
                record.Distance = values[15].trimmed().toDouble();
                record.Vs30 = values[16].trimmed().toDouble();

                record.Scale = values[4].toDouble();
                record.Horizontal1File = values[19].trimmed();
                record.Horizontal2File = values[20].trimmed();
                record.VerticalFile = values[21].trimmed();

                records.push_back(record);
                line = searchResultsStream.readLine();
            }
        }

        //Parsing scaled spectra
        if(line.contains("Scaled Spectra used in Search & Scaling"))
        {
            //skip header
            searchResultsStream.readLine();
            line = searchResultsStream.readLine();

            while(!line.isEmpty())
            {
                auto values = line.split(',');
                periods.push_back(values[0].toDouble());
                targetSpectrum.push_back(values[1].toDouble());
                meanSpectrum.push_back(values[2].toDouble());
                meanPlusSigmaSpectrum.push_back(values[3].toDouble());
                meanMinusSigmaSpectrum.push_back(values[4].toDouble());

                scaledSelectedSpectra.resize(values.size() - 5);
                for (int i = 5; i < values.size(); i++)
                {
                    scaledSelectedSpectra[i-5].push_back(values[i].toDouble());
                }
                line = searchResultsStream.readLine();
            }
        }
    }

    searchResultsFile.close();

    return records;
}

bool PEER_NGA_Records::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "ExistingPEER_Events";

    QJsonArray eventsArray;
    for (auto& record:currentRecords)
    {
        QJsonObject eventJson;
        QJsonArray recordsJsonArray;

        QJsonObject recordH1Json;
        //Adding Horizontal1 in dof 1 direction
        recordH1Json["fileName"] = record.Horizontal1File;
        recordH1Json["filePath"] = groundMotionsFolder.path();
        recordH1Json["dirn"] = 1;
        recordH1Json["factor"] = record.Scale;

        recordsJsonArray.append(recordH1Json);

        auto components = groundMotionsComponentsBox->currentData().value<GroundMotionComponents>();
        if(components == GroundMotionComponents::Two || components == GroundMotionComponents::Three)
        {
            QJsonObject recordH2Json;
            //Adding Horizontal2 in dof 2 direction
            recordH2Json["fileName"] = record.Horizontal2File;
            recordH2Json["filePath"] = groundMotionsFolder.path();
            recordH2Json["dirn"] = 2;
            recordH2Json["factor"] = record.Scale;

            recordsJsonArray.append(recordH2Json);
        }

        if(components == GroundMotionComponents::Three)
        {
            QJsonObject recordH3Json;
            //Adding Horizontal3 in dof 3 direction
            recordH3Json["fileName"] = record.VerticalFile;
            recordH3Json["filePath"] = groundMotionsFolder.path();
            recordH3Json["dirn"] = 3;
            recordH3Json["factor"] = record.Scale;

            recordsJsonArray.append(recordH3Json);
        }

        eventJson["Records"] = recordsJsonArray;
        eventJson["type"] = "PeerEvent";
        eventJson["EventClassification"] = "Earthquake";
        eventJson["name"] = QString("PEER-Record-") + QString::number(record.RSN);

        eventsArray.append(eventJson);
    }

    jsonObject["Events"] = eventsArray;

    auto spectrumJson = dynamic_cast<AbstractJsonSerializable*>(targetSpectrumDetails->currentWidget())->serialize();
    spectrumJson["SpectrumType"] = spectrumTypeComboBox->currentText();
    jsonObject["TargetSpectrum"] = spectrumJson;

    jsonObject["components"] = groundMotionsComponentsBox->currentText();

    jsonObject["records"] = nRecordsEditBox->text();

    jsonObject["magnitudeRange"] = magnitudeCheckBox->isChecked();
    jsonObject["magnitudeMin"] = magnitudeMin->text();
    jsonObject["magnitudeMax"] = magnitudeMax->text();

    jsonObject["distanceRange"] = distanceCheckBox->isChecked();
    jsonObject["distanceMin"] = distanceMin->text();
    jsonObject["distanceMax"] = distanceMax->text();

    jsonObject["vs30Range"] = vs30CheckBox->isChecked();
    jsonObject["vs30Min"] = vs30Min->text();
    jsonObject["vs30Max"] = vs30Max->text();

    return true;
}

bool PEER_NGA_Records::inputFromJSON(QJsonObject &jsonObject)
{
    if(jsonObject["TargetSpectrum"].isObject() && jsonObject["TargetSpectrum"].toObject().keys().contains("SpectrumType"))
    {
        auto targetSpectrumJson = jsonObject["TargetSpectrum"].toObject();
        spectrumTypeComboBox->setCurrentText(jsonObject["TargetSpectrum"].toObject()["SpectrumType"].toString());
        dynamic_cast<AbstractJsonSerializable*>(targetSpectrumDetails->currentWidget())->deserialize(jsonObject["TargetSpectrum"].toObject());
    }

    groundMotionsComponentsBox->setCurrentText(jsonObject["components"].toString());

    nRecordsEditBox->setText(jsonObject["records"].toString());

    auto magnitudeRange = jsonObject["magnitudeRange"].toBool();
    magnitudeCheckBox->setChecked(magnitudeRange);
    magnitudeMin->setText(jsonObject["magnitudeMin"].toString());
    magnitudeMin->setEnabled(magnitudeMin);
    magnitudeMax->setText(jsonObject["magnitudeMax"].toString());
    magnitudeMax->setEnabled(magnitudeMin);

    auto distanceRange = jsonObject["distanceRange"].toBool();
    distanceCheckBox->setChecked(distanceRange);
    distanceMin->setEnabled(distanceRange);
    distanceMin->setText(jsonObject["distanceMin"].toString());
    distanceMax->setEnabled(distanceRange);
    distanceMax->setText(jsonObject["distanceMax"].toString());

    auto vs30Range = jsonObject["vs30Range"].toBool();
    vs30CheckBox->setChecked(vs30Range);
    vs30Min->setEnabled(distanceRange);
    vs30Min->setText(jsonObject["vs30Min"].toString());
    vs30Max->setEnabled(distanceRange);
    vs30Max->setText(jsonObject["vs30Max"].toString());

    return true;
}

bool PEER_NGA_Records::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["Application"] = "ExistingPEER_Events";
    jsonObject["subtype"] = "PEER NGA Records";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    return true;
}

bool PEER_NGA_Records::inputAppDataFromJSON(QJsonObject &jsonObject)
{

    return true;
}

bool PEER_NGA_Records::copyFiles(QString &destDir)
{
    QDir recordsFolder(groundMotionsFolder.path());
    QDir destinationFolder(destDir);
    for (auto& record:currentRecords)
    {
        //Copying Horizontal1 file
        if (!QFile::copy(recordsFolder.filePath(record.Horizontal1File), destinationFolder.filePath(record.Horizontal1File)))
            return false;

        auto components = groundMotionsComponentsBox->currentData().value<GroundMotionComponents>();

        if(components == GroundMotionComponents::Two || components == GroundMotionComponents::Three)
        {

            //Copying Horizontal2 file
            if (!QFile::copy(recordsFolder.filePath(record.Horizontal2File), destinationFolder.filePath(record.Horizontal2File)))
                return false;
        }

        if(components == GroundMotionComponents::Three)
        {
            //Copying Vertical file
            if (!QFile::copy(recordsFolder.filePath(record.VerticalFile), destinationFolder.filePath(record.VerticalFile)))
                return false;
        }
    }

    return true;
}

void PEER_NGA_Records::onScalingComboBoxChanged(const int index)
{
    if(index == 0)
    {
        weightFunctionHeadingLabel->hide();
        weightFunctionLabel->hide();
        periodPointsLabel1->hide();
        periodPointsLineEdit->hide();
        periodPointsLabel2->hide();
        weightsLabel1->hide();
        weightsLineEdit->hide();
        weightsLabel2->hide();
        scalingPeriodLabel1->hide();
        scalingPeriodLineEdit->hide();
        scalingPeriodLabel2->hide();

        return;
    }
    else  // Show minimize MSE and scaling inputs
    {
        weightFunctionHeadingLabel->show();
        weightFunctionLabel->show();
        periodPointsLabel1->show();
        periodPointsLineEdit->show();
        periodPointsLabel2->show();
        weightsLabel1->show();
        weightsLineEdit->show();
        weightsLabel2->show();

        if(index == 2) // Show single period inputs
        {
            scalingPeriodLabel1->show();
            scalingPeriodLineEdit->show();
            scalingPeriodLabel2->show();
        }
        else
        {
            scalingPeriodLabel1->hide();
            scalingPeriodLineEdit->hide();
            scalingPeriodLabel2->hide();
        }
    }
}













