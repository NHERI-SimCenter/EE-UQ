#include "PeerNgaRecordsWidget.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include "PeerLoginDialog.h"
#include "MiniZip/ZipUtils.h"
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

PeerNgaRecordsWidget::PeerNgaRecordsWidget(QWidget *parent) : SimCenterAppWidget(parent), groundMotionsFolder(QDir::tempPath())
{
    setupUI();

    setupConnections();
}

void PeerNgaRecordsWidget::setupUI()
{
    auto layout = new QGridLayout(this);
    auto targetSpectrumGroup = new QGroupBox("Target Spectrum");

    auto targetSpectrumLayout = new QGridLayout(targetSpectrumGroup);
    targetSpectrumLayout->addWidget(new QLabel("Type"), 0, 0);
    auto spectrumTypeComboBox = new QComboBox();
    spectrumTypeComboBox->addItem("ASCE 7-10");
    targetSpectrumLayout->addWidget(spectrumTypeComboBox, 0, 1);

    targetSpectrumLayout->addWidget(new QLabel("S<sub>DS</sub>"), 1, 0);
    sdsEditBox = new QLineEdit("1.0");
    targetSpectrumLayout->addWidget(sdsEditBox, 1, 1);
    targetSpectrumLayout->addWidget(new QLabel("g"), 1, 2);

    targetSpectrumLayout->addWidget(new QLabel("S<sub>D1</sub>"), 2, 0);
    sd1EditBox = new QLineEdit("0.75");
    targetSpectrumLayout->addWidget(sd1EditBox, 2, 1);
    targetSpectrumLayout->addWidget(new QLabel("g"), 2, 2);

    targetSpectrumLayout->addWidget(new QLabel("T<sub>L</sub>"), 3, 0);
    tlEditBox = new QLineEdit("12.0");
    targetSpectrumLayout->addWidget(tlEditBox, 3, 1);
    targetSpectrumLayout->addWidget(new QLabel("g"), 3, 2);

    auto recordSelectionGroup = new QGroupBox("Record Selection");
    auto recordSelectionLayout = new QGridLayout(recordSelectionGroup);
    recordSelectionLayout->addWidget(new QLabel("Number of Records"), 0, 0);
    nRecordsEditBox = new QLineEdit("10");
    recordSelectionLayout->addWidget(nRecordsEditBox, 0, 1);

    //Magnitude Range
    magnitudeCheckBox = new QCheckBox("Magnitude");
    recordSelectionLayout->addWidget(magnitudeCheckBox, 1, 0);
    magnitudeMin = new QLineEdit("0.0");
    magnitudeMin->setEnabled(false);
    recordSelectionLayout->addWidget(magnitudeMin, 1, 1);
    magnitudeMax = new QLineEdit("8.0");
    magnitudeMax->setEnabled(false);
    recordSelectionLayout->addWidget(magnitudeMax, 1, 2);

    distanceCheckBox = new QCheckBox("Distance");
    recordSelectionLayout->addWidget(distanceCheckBox, 2, 0);
    distanceMin = new QLineEdit("0.0");
    distanceMin->setEnabled(false);
    recordSelectionLayout->addWidget(distanceMin, 2, 1);
    distanceMax = new QLineEdit("50.0");
    distanceMax->setEnabled(false);
    recordSelectionLayout->addWidget(distanceMax, 2, 2);
    recordSelectionLayout->addWidget(new QLabel("km"), 2, 3);

    vs30CheckBox = new QCheckBox("Vs30");
    recordSelectionLayout->addWidget(vs30CheckBox, 3, 0);
    vs30Min = new QLineEdit("150");
    vs30Min->setEnabled(false);
    recordSelectionLayout->addWidget(vs30Min, 3, 1);
    vs30Max = new QLineEdit("300.0");
    vs30Max->setEnabled(false);
    recordSelectionLayout->addWidget(vs30Max, 3, 2);
    recordSelectionLayout->addWidget(new QLabel("m/s"), 3, 3);

    selectRecordsButton = new QPushButton("Select Records");
    recordSelectionLayout->addWidget(selectRecordsButton, 4, 0, 1, 4);

    recordSelectionLayout->setRowStretch(recordSelectionLayout->rowCount(), 1);

    //Records Table
    recordsTable = new QTableWidget();

    layout->addWidget(targetSpectrumGroup, 0, 0);
    layout->addWidget(recordSelectionGroup, 0, 1);

    layout->addWidget(recordsTable, 1, 0, 1, 2);

    thePlottingWindow = new SimCenterGraphPlot(QString("Period (sec.)"), QString("Spectral Acceleration (g)"));
    layout->addWidget(thePlottingWindow, 0,3,2,1);
    layout->setRowStretch(layout->rowCount(), 1);
    layout->setColumnStretch(layout->columnCount(), 1);
}

void PeerNgaRecordsWidget::setupConnections()
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
        ZipUtils::UnzipFile(recordsFile, QDir(groundMotionsFolder.path()));
        processPeerRecords(QDir(groundMotionsFolder.path()));
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

    connect(&peerClient, &PeerNgaWest2Client::statusUpdated, this, &PeerNgaRecordsWidget::updateStatus);
}

void PeerNgaRecordsWidget::processPeerRecords(QDir resultFolder)
{
    if(!resultFolder.exists())
        return;

    clearSpectra();

    currentRecords = parseSearchResults(resultFolder.filePath("_SearchResults.csv"));
    setRecordsTable(currentRecords);

    thePlottingWindow->clear();
    plotSpectra();
}

void PeerNgaRecordsWidget::setRecordsTable(QList<PeerScaledRecord> records)
{
    recordsTable->clear();
    int row = 0;
    recordsTable->setRowCount(records.size());
    recordsTable->setColumnCount(4);
    recordsTable->setHorizontalHeaderLabels(QStringList({"RSN","Scale", "Earthquake", "Station"}));
    for(auto& record: records)
    {
        recordsTable->setItem(row, 0, new QTableWidgetItem(QString::number(record.RSN)));
        recordsTable->setItem(row, 1, new QTableWidgetItem(QString::number(record.scale)));
        recordsTable->setItem(row, 2, new QTableWidgetItem(record.Earthquake));
        recordsTable->setItem(row, 3, new QTableWidgetItem(record.Station));

        row++;
    }
    recordsTable->resizeColumnsToContents();
}

void PeerNgaRecordsWidget::clearSpectra()
{
    periods.clear();
    meanSpectrum.clear();
    meanPlusSigmaSpectrum.clear();
    meanMinusSigmaSpectrum.clear();
    targetSpectrum.clear();
    scaledSelectedSpectra.clear();
}

void PeerNgaRecordsWidget::plotSpectra()
{
    //Spectra can be plotted here using the data in
    //periods, targetSpectrum, meanSpectrum, meanPlusSigmaSpectrum, meanMinusSigmaSpectrum, scaledSelectedSpectra


    for (auto i : scaledSelectedSpectra) {
        thePlottingWindow->addLine(periods, i, 1, 125, 125, 125);
    }

    thePlottingWindow->addLine(periods, meanSpectrum, 3, 255, 0, 0);
    thePlottingWindow->addLine(periods, meanMinusSigmaSpectrum, 1, 255, 0, 0);
    thePlottingWindow->addLine(periods, meanPlusSigmaSpectrum, 1, 255, 0, 0);

    thePlottingWindow->addLine(periods, targetSpectrum, 3, 0, 0, 255);

}

void PeerNgaRecordsWidget::updateStatus(QString status)
{
    if(this->parent())
    {
        auto topWidget = this->parent();
        while(topWidget->parent()) topWidget = topWidget->parent();

        auto statusBar = static_cast<QMainWindow*>(topWidget)->statusBar();
        if (statusBar)
            statusBar->showMessage(status, 5000);
    }
}

void PeerNgaRecordsWidget::selectRecords()
{
    QVariant magnitudeRange;
    if(magnitudeCheckBox->checkState() == Qt::Checked)
        magnitudeRange.setValue(qMakePair(magnitudeMin->text().toDouble(), magnitudeMax->text().toDouble()));

    QVariant distanceRange;
    if(distanceCheckBox->checkState() == Qt::Checked)
        distanceRange.setValue(qMakePair(distanceMin->text().toDouble(), distanceMax->text().toDouble()));

    QVariant vs30Range;
    if(vs30CheckBox->checkState() == Qt::Checked)
        vs30Range.setValue(qMakePair(vs30Min->text().toDouble(), vs30Max->text().toDouble()));

    peerClient.selectRecords(sdsEditBox->text().toDouble(),
                             sd1EditBox->text().toDouble(),
                             tlEditBox->text().toDouble(),
                             nRecordsEditBox->text().toInt(), magnitudeRange, distanceRange, vs30Range);
}

QList<PeerScaledRecord> PeerNgaRecordsWidget::parseSearchResults(QString searchResultsFilePath)
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
                record.scale = values[4].toDouble();
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

bool PeerNgaRecordsWidget::outputToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["type"] = "ExistingPEER_Events";

    bool result = true;
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
        recordH1Json["factor"] = record.scale;

        recordsJsonArray.append(recordH1Json);

        eventJson["Records"] = recordsJsonArray;
        eventJson["type"] = "PeerEvent";
        eventJson["EventClassification"] = "Earthquake";
        eventJson["name"] = QString("PEER-Record-") + QString::number(record.RSN);

        eventsArray.append(eventJson);
    }

    jsonObject["Events"] = eventsArray;

    return result;
}

bool PeerNgaRecordsWidget::inputFromJSON(QJsonObject &jsonObject)
{

    return true;
}

bool PeerNgaRecordsWidget::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["Application"] = "ExistingPEER_Events";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    return true;
}

bool PeerNgaRecordsWidget::inputAppDataFromJSON(QJsonObject &jsonObject)
{

    return true;
}

bool PeerNgaRecordsWidget::copyFiles(QString &destDir)
{
    QDir recordsFolder(groundMotionsFolder.path());
    QDir destinationFolder(destDir);
    for (auto& record:currentRecords)
    {
        //Copying Horizontal1 in dof 1 direction
        if (!QFile::copy(recordsFolder.filePath(record.Horizontal1File), destinationFolder.filePath(record.Horizontal1File)))
            return false;
    }

    return true;
}
