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
#include "NoSpectrumUniform.h"
#include "UserSpectrumWidget.h"
#include "USGSTargetWidget.h"
#include "NSHMPTarget.h"
#include "NSHMPDeagg.h"
#include <GoogleAnalytics.h>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include "SpectrumFromRegionalSurrogate.h"
#include <QWebEngineView>

PEER_NGA_Records::PEER_NGA_Records(GeneralInformationWidget* generalInfoWidget, QWidget *parent) : SimCenterAppWidget(parent), groundMotionsFolder(QDir::tempPath())
{
    setupUI(generalInfoWidget);

    setupConnections();
}

PEER_NGA_Records::~PEER_NGA_Records()
{
    //coverageImage->deleteLater();
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

    targetSpectrumLayout->addWidget(new QLabel("Type      "), 0, 0);
    spectrumTypeComboBox = new QComboBox();

    targetSpectrumLayout->addWidget(spectrumTypeComboBox, 0, 1, Qt::AlignLeft);
    spectrumTypeComboBox->addItem("Design Spectrum (ASCE 7-10)");
    spectrumTypeComboBox->addItem("User Specified");
    spectrumTypeComboBox->addItem("Design Spectrum (USGS Web Service)");
    spectrumTypeComboBox->addItem("Uniform Hazard Spectrum (USGS NSHMP)");
    spectrumTypeComboBox->addItem("Conditional Mean Spectrum (USGS Disagg.)");
    spectrumTypeComboBox->addItem("Spectrum from Hazard Surrogate");
    spectrumTypeComboBox->addItem("No Spectrum - Uniform IMs");

    targetSpectrumDetails = new QStackedWidget(this);
    targetSpectrumLayout->addWidget(targetSpectrumDetails, 1, 0, 1, 3);
    auto asce710Target = new ASCE710Target(this);
    targetSpectrumDetails->addWidget(asce710Target);
    userSpectrumTarget = new UserSpectrumWidget(this);
    targetSpectrumDetails->addWidget(userSpectrumTarget);
    auto usgsSpectrumTarget = new USGSTargetWidget(generalInfoWidget, this);
    targetSpectrumDetails->addWidget(usgsSpectrumTarget);
    auto nshmpTarget = new NSHMPTarget(generalInfoWidget, this);
    targetSpectrumDetails->addWidget(nshmpTarget);
    auto nshmpDeagg = new NSHMPDeagg(generalInfoWidget, this);
    targetSpectrumDetails->addWidget(nshmpDeagg);
    spectrumSurrogate = new SpectrumFromRegionalSurrogate(this);
    targetSpectrumDetails->addWidget(spectrumSurrogate);
    auto noSpect =  new NoSpectrumUniform(this);
    targetSpectrumDetails->addWidget(noSpect);

    recordSelectionGroup = new QGroupBox("Record Selection");
    recordSelectionLayout = new QGridLayout(recordSelectionGroup);
    recordSelectionLayout->addWidget(new QLabel("Number of Records"), 0, 0);
    nRecordsEditBox = new QLineEdit("16");
    nRecordsEditBox->setValidator(positiveIntegerValidator);
    recordSelectionLayout->addWidget(nRecordsEditBox, 0, 1);

    // Fault Type
    faultTypeBox = new QComboBox();
    faultTypeBox->addItem("All Types");
    faultTypeBox->addItem("Strike Slip (SS)");
    faultTypeBox->addItem("Normal/Oblique");
    faultTypeBox->addItem("Reverse/Oblique");
    faultTypeBox->addItem("SS+Normal");
    faultTypeBox->addItem("SS+Reverse");
    faultTypeBox->addItem("Normal+Reverse");
    recordSelectionLayout->addWidget(new QLabel("Fault Type"), 1, 0);
    recordSelectionLayout->addWidget(faultTypeBox, 1, 1);

    // Pulse Type
    pulseBox = new QComboBox();
    pulseBox->addItem("All");
    pulseBox->addItem("Only Pulse-like");
    pulseBox->addItem("No Pulse-like");
    recordSelectionLayout->addWidget(new QLabel("Pulse"), 2, 0);
    recordSelectionLayout->addWidget(pulseBox, 2, 1);

    //Magnitude Range
    magnitudeCheckBox = new QCheckBox("Magnitude");
    recordSelectionLayout->addWidget(magnitudeCheckBox, 3, 0);
    magnitudeMin = new QLineEdit("5.0");
    magnitudeMin->setEnabled(false);
    magnitudeMin->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(magnitudeMin, 3, 1);
    magnitudeMax = new QLineEdit("8.0");
    magnitudeMax->setEnabled(false);
    magnitudeMax->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(magnitudeMax, 3, 2);

    distanceCheckBox = new QCheckBox("Distance");
    recordSelectionLayout->addWidget(distanceCheckBox, 4, 0);
    distanceMin = new QLineEdit("0.0");
    distanceMin->setEnabled(false);
    distanceMin->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(distanceMin, 4, 1);
    distanceMax = new QLineEdit("50.0");
    distanceMax->setEnabled(false);
    distanceMax->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(distanceMax, 4, 2);
    recordSelectionLayout->addWidget(new QLabel("km"), 4, 3);

    vs30CheckBox = new QCheckBox("Vs30");
    recordSelectionLayout->addWidget(vs30CheckBox, 5, 0);
    vs30Min = new QLineEdit("150.0");
    vs30Min->setEnabled(false);
    vs30Min->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(vs30Min, 5, 1);
    vs30Max = new QLineEdit("300.0");
    vs30Max->setEnabled(false);
    vs30Max->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(vs30Max, 5, 2);
    recordSelectionLayout->addWidget(new QLabel("m/s"), 5, 3);

    durationCheckBox = new QCheckBox("D5-95");
    recordSelectionLayout->addWidget(durationCheckBox, 6, 0);
    durationMin = new QLineEdit("0.0");
    durationMin->setEnabled(false);
    durationMin->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(durationMin, 6, 1);
    durationMax = new QLineEdit("20.0");
    durationMax->setEnabled(false);
    durationMax->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(durationMax, 6, 2);
    recordSelectionLayout->addWidget(new QLabel("sec"), 6, 3);

//#ifdef _WIN32
//    targetSpectrumGroup->setMaximumHeight(200);
//    recordSelectionGroup->setMaximumHeight(200);
//#else
    targetSpectrumLayout->setRowStretch(2,1);
    targetSpectrumLayout->setColumnStretch(2,1);
    recordSelectionLayout->setRowStretch(7, 1);
//#endif

    auto scalingGroup = new QGroupBox("Scaling/Selection Criteria");
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

    weightFunctionHeadingLabel = new QLabel("Selection Error Weight Function");
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

    // User-defined output directory
    auto outdirGroup = new QGroupBox("Temporary records Directory");
    auto outdirLayout = new QGridLayout(outdirGroup);
    // add stuff to enter Output Directory
    QLabel *labelOD = new QLabel("Temporary records Directory");
    outdirLE = new QLineEdit;
    outdirLE->setPlaceholderText("(Opional) " + groundMotionsFolder.path());
    QPushButton *chooseOutputDirectoryButton = new QPushButton();
    chooseOutputDirectoryButton->setText(tr("Choose"));
    connect(chooseOutputDirectoryButton,SIGNAL(clicked()),this,SLOT(chooseOutputDirectory()));
    outdirLayout->addWidget(labelOD,0,0);
    outdirLayout->addWidget(outdirLE,0,2);
    outdirLayout->addWidget(chooseOutputDirectoryButton, 0, 4);

    //Records Table
    recordsTable = new QTableWidget();
    recordsTable->setHidden(true);

    //Ground Motions
    auto groundMotionsGroup = new QGroupBox("Ground Motion Components");
    auto groundMotionsLayout = new QGridLayout(groundMotionsGroup);
    groundMotionsComponentsBox = new QComboBox();
    /*
    groundMotionsComponentsBox->addItem("One (Horizontal)", GroundMotionComponents::One);
    groundMotionsComponentsBox->addItem("Two (Horizontal)", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("Three (Horizontal & Vertical)", GroundMotionComponents::Three);
    */
    groundMotionsComponentsBox->addItem("SRSS", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("RotD100", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("RotD50", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("GeoMean", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("H1", GroundMotionComponents::One);
    groundMotionsComponentsBox->addItem("H2", GroundMotionComponents::One);
    groundMotionsComponentsBox->addItem("V", GroundMotionComponents::Three);

    // Suite Averge
    suiteAverageBox = new QComboBox();
    suiteAverageBox->addItem("Arithmetic");
    suiteAverageBox->addItem("Geometric");

    groundMotionsLayout->addWidget(new QLabel("Acceleration Components"), 0, 0);
    groundMotionsLayout->addWidget(groundMotionsComponentsBox, 0, 1);
    groundMotionsLayout->addWidget(new QLabel("Suite Average"), 1, 0);
    groundMotionsLayout->addWidget(suiteAverageBox, 1, 1);
    recordsTable->setMinimumHeight(200);
    groundMotionsLayout->addWidget(recordsTable, 2, 0, 1, 2);
    groundMotionsLayout->setRowStretch(2, 1);

    progressBar = new QProgressBar();
    progressBar->setRange(0,0);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setHidden(true);

    groundMotionsLayout->addWidget(progressBar, 3, 0, 1, 2);

    layout->addWidget(targetSpectrumGroup, 0, 0);
    layout->addWidget(recordSelectionGroup, 0, 1);
    // Output directory group location
    layout->addWidget(outdirGroup, 1, 0, 1, 2);
    layout->addWidget(groundMotionsGroup, 2, 0, 1, 2);
    layout->addWidget(scalingGroup, 3, 0, 1, 2);

    auto peerCitation = new QLabel("This tool uses PEER NGA West 2 Ground Motions Database. "
                                   "Users should cite the database as follows: PEER 2013/03 – PEER NGA-West2 Database, "
                                   "Timothy D. Ancheta, Robert B. Darragh, Jonathan P. Stewart, Emel Seyhan, Walter J. Silva, "
                                   "Brian S.J. Chiou, Katie E. Wooddell, Robert W. Graves, Albert R. Kottke, "
                                   "David M. Boore, Tadahiro Kishida, and Jennifer L. Donahue.");

    peerCitation->setWordWrap(true);
    layout->addWidget(peerCitation, 4, 0, 1, 3);

    //add record selection plot
    layout->addWidget(&recordSelectionPlot, 0,3,4,1);
    recordSelectionPlot.setHidden(true);


    //coverageImage = new QLabel();
    //layout->addWidget(coverageImage, 0,3,4,1);
    //coverageImage->setHidden(true);

    // sy - **NOTE** QWebEngineView display is VERY SLOW in debug mode / Max size of figure is limited to 2MB
    coverageImage = new QWebEngineView();
    coverageImage->page()->setBackgroundColor(Qt::transparent);
    coverageImage->setHtml("Loading coverage image...");
    layout->addWidget(coverageImage, 0,3,4,1);
    coverageImage->setHidden(true);
    //coverageImage->load(QUrl::fromLocalFile(("C:/Users/SimCenter/AppData/Local/Temp.oohpbs/gridIM_coverage.html")));


    layout->setRowStretch(0,1);
    //layout->setRowStretch(layout->rowCount(), 1);
    layout->setColumnStretch(layout->columnCount(), 1);
}

void PEER_NGA_Records::setupConnections()
{
    // Output directory check
    if(outdirpath.compare("NULL") == 0)
        this->chooseOutputDirectory();

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
        // auto tempRecordsDir = QDir(groundMotionsFolder.path());
        // Adding user-defined output directory
        RecordsDir = this->outdirLE->text();
        if (RecordsDir.isEmpty()) {
            RecordsDir = groundMotionsFolder.path();
        }
        auto tempRecordsDir = QDir(RecordsDir);
        //Cleaning up previous search results
        if(tempRecordsDir.exists("_SearchResults.csv"))
            tempRecordsDir.remove("_SearchResults.csv");
        if(tempRecordsDir.exists("_readME.txt"))
            tempRecordsDir.remove("_readME.txt");
        QDir it(RecordsDir, {"grid_IM*"});
        for(const QString & filename: it.entryList()){
            it.remove(filename);
        }


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

    connect(durationCheckBox, &QCheckBox::clicked, this, [this](bool checked){
        durationMin->setEnabled(checked);
        durationMax->setEnabled(checked);
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
        if(spectrumTypeComboBox->currentText().contains("No Spectrum")) {
            double newHeight =  targetSpectrumDetails->height();
            double newWidth =  targetSpectrumDetails->width();
            newWidth += recordSelectionGroup->width();

            recordSelectionGroup->setVisible(false);
            //targetSpectrumDetails->resize(newHeight, newWidth);
            targetSpectrumDetails->setMinimumWidth(newWidth);
            //widget->resize(165, widget->height());

        } else {
            recordSelectionGroup->setVisible(true);
            targetSpectrumDetails->setMinimumWidth(10); // some random number
        }
        return;
    });


    for(int i = 0; i < targetSpectrumDetails->count(); i++)
    {
        auto targetWidget = reinterpret_cast<AbstractTargetWidget*>(targetSpectrumDetails->widget(i));
        connect(targetWidget, &AbstractTargetWidget::statusUpdated, this, &PEER_NGA_Records::updateStatus);
    }


    connect(scalingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onScalingComboBoxChanged(int)));

    connect(spectrumSurrogate, &SpectrumFromRegionalSurrogate::spectrumSaved, this, &PEER_NGA_Records::switchUserDefined);
}

void PEER_NGA_Records::processPeerRecords(QDir resultFolder)
{
    statusMessage(QString("Parsing Downloaded Records"));
    if(!resultFolder.exists())
        return;


    QString readMePathString = RecordsDir + QDir::separator() + QString("_readME.txt");
    QFileInfo readMeInfo(readMePathString);
    if (readMeInfo.exists()) {
        QFile readMeFile(readMePathString);
        QString line("");
        if (readMeFile.open(QIODevice::ReadOnly)) {
           QTextStream in(&readMeFile);
           while (!in.atEnd()) {
              line = in.readLine();
           }
           readMeFile.close();
        }
        if (line.contains("No records")) {
            errorMessage(QString(QString("Failed to download PEER NGA records: ") + line));
            errorMessage(QString("The limit can be 100 per day, 200 per week, 400 per month"));
            return;
        } else {
            infoMessage(QString(QString("Message from PEER NGA: ") + line));
            }
    }


    clearSpectra();

    currentRecords = parseSearchResults(resultFolder.filePath("_SearchResults.csv"));
    setRecordsTable(currentRecords);

    plotSpectra();
    statusMessage(QString(""));
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
        addTableItem(row, 1, QString::number(record.Scale, 'g', 2));
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

    coverageImage->setHidden(true);
    recordSelectionPlot.setHidden(true);
    if (spectrumTypeComboBox->currentIndex()!=6) {
        recordSelectionPlot.setHidden(false);
        recordSelectionPlot.setSelectedSpectra(periods, scaledSelectedSpectra);
        recordSelectionPlot.setMean(periods, meanSpectrum);
        recordSelectionPlot.setMeanPlusSigma(periods, meanPlusSigmaSpectrum);
        recordSelectionPlot.setMeanMinusSigma(periods, meanMinusSigmaSpectrum);
        recordSelectionPlot.setTargetSpectrum(periods, targetSpectrum);

        auto size = recordSelectionPlot.size();
        size.setWidth(size.height());
        recordSelectionPlot.setMinimumSize(size);
    } else {
        coverageImage->setHidden(false);
    }

}


void PEER_NGA_Records::updateStatus(QString status)
{
    statusMessage(status);

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

    QVariant durationRange;
    if(durationCheckBox->checkState() == Qt::Checked)
        durationRange.setValue(qMakePair(durationMin->text().toDouble(), durationMax->text().toDouble()));

    additionalScaling.clear();
    if(targetSpectrumDetails->currentIndex() == 0)
    {
        auto asce710widget = reinterpret_cast<ASCE710Target*>(targetSpectrumDetails->currentWidget());
        peerClient.selectRecords(asce710widget->sds(),
                                 asce710widget->sd1(),
                                 asce710widget->tl(),
                                 nRecordsEditBox->text().toInt(),
				 magnitudeRange,
				 distanceRange,
                 vs30Range,durationRange,groundMotionsComponentsBox->currentIndex()+1,suiteAverageBox->currentIndex(),faultTypeBox->currentIndex()+1,pulseBox->currentIndex()+1);

        // _ no additional scaling
        additionalScaling = QVector(nRecordsEditBox->text().toInt(),1.0);
    }
    else if(targetSpectrumDetails->currentIndex() == 6) // no spectrum (uniform
    {
        auto unifrom_widget = reinterpret_cast<NoSpectrumUniform*>(targetSpectrumDetails->currentWidget());
        updateStatus("Retrieving ground motion RSN ...");
        QStringList RSN;
        QString imagePath;
        RecordsDir = this->outdirLE->text();
        if (RecordsDir.isEmpty()) {
            RecordsDir = groundMotionsFolder.path();
        }
        unifrom_widget->getRSN(RecordsDir, RSN, additionalScaling, imagePath); // This will run a python script
        // additionalScaling are given in "sorted" RSN older.
        RSN.removeAll(QString(""));
        if (RSN.isEmpty()) {
            return;
            // TO ADD error messages here
        } else {
            peerClient.selectRecords(RSN);
            //return;
        }
        //TEMP TESTING

        QFile searchImageFile(imagePath);
        if(searchImageFile.exists()) {
            //coverageImage->setPixmap(QPixmap(imagePath));
            coverageImage->load(QUrl::fromLocalFile((imagePath)));
            coverageImage->show();
        } else {
            coverageImage->setHidden(true);
        }

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
            peerClient.selectRecords(userTargetWidget->spectrum(), nRecordsEditBox->text().toInt(), magnitudeRange, distanceRange, vs30Range, durationRange,
                                     groundMotionsComponentsBox->currentIndex()+1, suiteAverageBox->currentIndex(), faultTypeBox->currentIndex()+1, pulseBox->currentIndex()+1);
        else
        {
            progressBar->setHidden("True");
            selectRecordsButton->setEnabled(true);
            selectRecordsButton->setDown(false);
        }

        // _ no additional scaling
        additionalScaling = QVector(nRecordsEditBox->text().toInt(),1.0);
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
        int ng = 0;
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

                record.Scale = values[4].toDouble()*additionalScaling[ng];
                record.Horizontal1File = values[19].trimmed();
                record.Horizontal2File = values[20].trimmed();
                record.VerticalFile = values[21].trimmed();

                records.push_back(record);
                line = searchResultsStream.readLine();
                ng++;
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
                    scaledSelectedSpectra[i-5].push_back(values[i].toDouble()*additionalScaling[i-5]);
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
    int numRecords = 0;
    for (auto& record:currentRecords)
    {
        numRecords++;
	
        QJsonObject eventJson;
        QJsonArray recordsJsonArray;

        QJsonObject recordH1Json;
        //Adding Horizontal1 in dof 1 direction
        recordH1Json["fileName"] = record.Horizontal1File;
        // recordH1Json["filePath"] = groundMotionsFolder.path();
        recordH1Json["filePath"] = RecordsDir;
        recordH1Json["dirn"] = 1;
        recordH1Json["factor"] = record.Scale;

        recordsJsonArray.append(recordH1Json);

        auto components = groundMotionsComponentsBox->currentData().value<GroundMotionComponents>();
        if(components == GroundMotionComponents::Two || components == GroundMotionComponents::Three)
        {
            QJsonObject recordH2Json;
            //Adding Horizontal2 in dof 2 direction
            recordH2Json["fileName"] = record.Horizontal2File;
            // recordH2Json["filePath"] = groundMotionsFolder.path();
            recordH2Json["filePath"] = RecordsDir;
            recordH2Json["dirn"] = 2;
            recordH2Json["factor"] = record.Scale;

            recordsJsonArray.append(recordH2Json);
        }

        if(components == GroundMotionComponents::Three)
        {
            QJsonObject recordH3Json;
            //Adding Horizontal3 in dof 3 direction
            recordH3Json["fileName"] = record.VerticalFile;
            // recordH3Json["filePath"] = groundMotionsFolder.path();
            recordH3Json["filePath"] = RecordsDir;
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
    /*
    if (numRecords == 0) {
      statusMessage(QString("PEER-NGA no motions yet selected"));      
      switch( QMessageBox::question( 
            this, 
            tr("PEER-NGA"), 
            tr("PEER-NGA has detected that no motions have been selected. If you are trying to Run a Workflow, the workflow will FAIL. To select motions, return to the PEER-NGA EVENT and press the 'Select Records' Button. Do you wish to continue anyway?"), 
            QMessageBox::Yes | 
            QMessageBox::No,
            QMessageBox::Yes ) )
	{
	case QMessageBox::Yes:
	  break;
	case QMessageBox::No:
	  return false;
	  break;
	default:

	  break;
	}
    }
    */
    
    jsonObject["Events"] = eventsArray;

    if (spectrumTypeComboBox->currentText() != QString("No Spectrum - Uniform IMs"))
    {
        auto spectrumJson = dynamic_cast<AbstractJsonSerializable*>(targetSpectrumDetails->currentWidget())->serialize();
        spectrumJson["SpectrumType"] = spectrumTypeComboBox->currentText();
        jsonObject["TargetSpectrum"] = spectrumJson;

        jsonObject["scaling"] = scalingComboBox->currentText();
        jsonObject["singlePeriod"] = scalingPeriodLineEdit->text();
        jsonObject["periodPoints"] = periodPointsLineEdit->text();
        jsonObject["weights"] = weightsLineEdit->text();

        jsonObject["components"] = groundMotionsComponentsBox->currentText();
        jsonObject["faultType"] = faultTypeBox->currentText();
        jsonObject["pulse"] = pulseBox->currentText();

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

        jsonObject["durationRange"] = durationCheckBox->isChecked();
        jsonObject["durationMin"] = durationMin->text();
        jsonObject["durationMax"] = durationMax->text();
    }

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

    scalingComboBox->setCurrentText(jsonObject["scaling"].toString());
    scalingPeriodLineEdit->setText(jsonObject["singlePeriod"].toString());
    periodPointsLineEdit->setText(jsonObject["periodPoints"].toString());
    weightsLineEdit->setText(jsonObject["weights"].toString());

    groundMotionsComponentsBox->setCurrentText(jsonObject["components"].toString());
    faultTypeBox->setCurrentText(jsonObject["faultType"].toString());
    pulseBox->setCurrentText(jsonObject["pulse"].toString());

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
    vs30Min->setEnabled(vs30Range);
    vs30Min->setText(jsonObject["vs30Min"].toString());
    vs30Max->setEnabled(vs30Range);
    vs30Max->setText(jsonObject["vs30Max"].toString());

    auto durationRange = jsonObject["durationRange"].toBool();
    durationCheckBox->setChecked(durationRange);
    durationMin->setEnabled(durationMin);
    durationMin->setText(jsonObject["durationMin"].toString());
    durationMax->setEnabled(durationRange);
    durationMax->setText(jsonObject["durationMax"].toString());

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
    // QDir recordsFolder(groundMotionsFolder.path());
    QDir recordsFolder(RecordsDir);
    QDir destinationFolder(destDir);

    bool ok = true;
    QString msg;
    int count = 0;
    for (auto& record:currentRecords)
      {
        //Copying Horizontal1 file
	if (!QFile::copy(recordsFolder.filePath(record.Horizontal1File), destinationFolder.filePath(record.Horizontal1File))) {
	  msg = "PEER NGA: failed to record:" +  recordsFolder.filePath(record.Horizontal1File);
	  ok = false;
	  break;
	}
	
	
        auto components = groundMotionsComponentsBox->currentData().value<GroundMotionComponents>();
	
        if(components == GroundMotionComponents::Two || components == GroundMotionComponents::Three)
	  {
	    
            //Copying Horizontal2 file
	    if (!QFile::copy(recordsFolder.filePath(record.Horizontal2File), destinationFolder.filePath(record.Horizontal2File))) {
	      msg = "PEER NGA: failed to record:" +  recordsFolder.filePath(record.Horizontal2File);
	      ok = false;
	      break; 	    
	    }
	  }

	if(components == GroundMotionComponents::Three)
	  {
            //Copying Vertical file
	    if (!QFile::copy(recordsFolder.filePath(record.VerticalFile), destinationFolder.filePath(record.VerticalFile))) {
	      msg = "PEER NGA: failed to record:" +  recordsFolder.filePath(record.VerticalFile);
	      ok = false;
	      break;
	    }
	  }
	count++;
      }


    //FMKFMK
    if (ok == false || count == 0) {
      statusMessage(QString("PEER-NGA no motions Downloaded"));      
      switch( QMessageBox::question( 
            this, 
            tr("PEER-NGA"), 
            tr("PEER-NGA has detected that no motions have been downloaded. You may have requested too many motions or you did not run the 'Select Records' after entering your search criteria. If you  trying to Run a Workflow, the workflow will FAIL to run or you will be presented with NANs (not a number) and zeroes. To select motions, return to the PEER-NGA EVENT and press the 'Select Records' Button. If you have pressed this button and see this message you do not have the priviledges with PEER that will allow you to download the number of motions you have specified, the typical limit is 100 per day. Do you wish to continue anyway?"), 
            QMessageBox::Yes | 
            QMessageBox::No,
            QMessageBox::Yes ) )
	{
	case QMessageBox::Yes:
	  break;
	case QMessageBox::No:
	  return false;
	  break;
	default:

	  break;
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

void
PEER_NGA_Records::setOutputDirectory(QString dirpath) {
    outdirLE->setText(dirpath);
    return;
}

void
PEER_NGA_Records::chooseOutputDirectory(void) {
    outdirpath=QFileDialog::getExistingDirectory(this,tr("Output Folder"));
    if(outdirpath.isEmpty())
    {
        outdirpath = "NULL";
        return;
    }
    this->setOutputDirectory(outdirpath);

}

void PEER_NGA_Records::switchUserDefined(QString dirName, QString fileName) {
    // switch user defined
    targetSpectrumDetails->setCurrentIndex(1);
    spectrumTypeComboBox->setCurrentIndex(1);
    // load the csv file in
    userSpectrumTarget->loadSpectrum(dirName+QDir::separator()+fileName);
}












