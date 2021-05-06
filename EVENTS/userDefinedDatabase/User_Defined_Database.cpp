#include "User_Defined_Database.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
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
#include <QHeaderView>
#include <QTableWidget>
#include <QMainWindow>
#include <QThread>
#include "../peerNGA/ASCE710Target.h"
#include "../peerNGA/UserSpectrumWidget.h"
#include "../peerNGA/USGSTargetWidget.h"
#include "../peerNGA/NSHMPTarget.h"
#include <GoogleAnalytics.h>
#include <QLineEdit>
#include <QFileDialog>

User_Defined_Database::User_Defined_Database(GeneralInformationWidget* generalInfoWidget, QWidget *parent) : SimCenterAppWidget(parent), groundMotionsFolder(QDir::tempPath())
{
    setDefaultValues();

    setupUI(generalInfoWidget);    

    setupConnections();
}

void User_Defined_Database::setupUI(GeneralInformationWidget* generalInfoWidget)
{
    auto layout = new QGridLayout(this);

    auto positiveIntegerValidator = new QIntValidator();
    positiveIntegerValidator->setBottom(1);

    auto positiveDoubleValidator = new QDoubleValidator();
    positiveDoubleValidator->setBottom(0.0);

    // User-defined database directory
    auto infileGroup = new QGroupBox("User Specified Database");
    auto infileLayout = new QGridLayout(infileGroup);
    QLabel *labelFF = new QLabel("Flatfile");
    flatfileLE = new QLineEdit;
    QPushButton *chooseFlatfileButton = new QPushButton();
    chooseFlatfileButton->setText(tr("Choose"));
    connect(chooseFlatfileButton,SIGNAL(clicked()),this,SLOT(chooseFlatfile()));
    infileLayout->addWidget(labelFF,0,0);
    infileLayout->addWidget(flatfileLE,0,2);
    infileLayout->addWidget(chooseFlatfileButton, 0, 4);

    /*
    QLabel *labelGM = new QLabel("Record Folder");
    gmDataLE = new QLineEdit;
    QPushButton *chooseGMButton = new QPushButton();
    chooseGMButton->setText(tr("Choose"));
    connect(chooseGMButton,SIGNAL(clicked()),this,SLOT(chooseGM()));
    infileLayout->addWidget(labelGM,1,0);
    infileLayout->addWidget(gmDataLE,1,2);
    infileLayout->addWidget(chooseGMButton, 1, 4);
    */

    auto previewGroup = new QGroupBox("Preview Records");
    auto previewLayout = new QGridLayout(previewGroup);
    flatfileSpreadSheet = new QTableWidget();
    connect(flatfileSpreadSheet, SIGNAL(cellClicked(int,int)), this, SLOT(onSpreadsheetCellClicked(int,int)));
    connect(flatfileSpreadSheet, SIGNAL(cellEntered(int,int)), this,SLOT(onSpreadsheetCellClicked(int,int)));
    connect(flatfileSpreadSheet, SIGNAL(cellChanged(int,int)), this,SLOT(onSpreadsheetCellChanged(int,int)));
    previewLayout->addWidget(flatfileSpreadSheet,0,0);
    // record table (default)
    flatfileSpreadSheet->setColumnCount(9);
    flatfileSpreadSheet->setRowCount(1);
    QStringList tableHeadings;
    foreach (const QString str, defaultCol)
        tableHeadings << str;
    flatfileSpreadSheet->setHorizontalHeaderLabels(tableHeadings);

    //infileGroup->setMaximumHeight(60);
    //previewGroup->setMaximumHeight(200);

    auto targetSpectrumGroup = new QGroupBox("Target Intensity Measures");

    auto targetSpectrumLayout = new QGridLayout(targetSpectrumGroup);
    targetSpectrumLayout->setColumnMinimumWidth(1, 100);
    targetSpectrumLayout->setColumnMinimumWidth(2, 30);

    targetSpectrumLayout->addWidget(new QLabel("Type"), 0, 0);
    spectrumTypeComboBox = new QComboBox();

    targetSpectrumLayout->addWidget(spectrumTypeComboBox, 0, 1);
    spectrumTypeComboBox->addItem("Design Spectrum (ASCE 7-10)");
    spectrumTypeComboBox->addItem("User Specified Spectrum");
    spectrumTypeComboBox->addItem("Design Spectrum (USGS Web Service)");
    spectrumTypeComboBox->addItem("Uniform Hazard Spectrum (USGS NSHMP)");
    //spectrumTypeComboBox->addItem("Grid Intensity Measures");
    //spectrumTypeComboBox->addItem("General Conditional Intensity Measure");

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
    targetSpectrumLayout->setRowStretch(1,1);

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

    sfCheckBox = new QCheckBox("Scaling Factor");
    recordSelectionLayout->addWidget(sfCheckBox, 4, 0);
    sfMin = new QLineEdit("0.1");
    sfMin->setEnabled(false);
    sfMin->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(sfMin, 4, 1);
    sfMax = new QLineEdit("10.0");
    sfMax->setEnabled(false);
    sfMax->setValidator(positiveDoubleValidator);
    recordSelectionLayout->addWidget(sfMax, 4, 2);
    recordSelectionLayout->setRowStretch(5,1);

   // targetSpectrumGroup->setMaximumHeight(200);
   // recordSelectionGroup->setMaximumHeight(200);

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

    // User-defined output directory
    auto outdirGroup = new QGroupBox("Output Directory");
    auto outdirLayout = new QGridLayout(outdirGroup);
    // add stuff to enter Output Directory
    QLabel *labelOD = new QLabel("Output Directory");
    outdirLE = new QLineEdit;
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
    auto groundMotionsGroup = new QGroupBox("Ground Motions");
    auto groundMotionsLayout = new QGridLayout(groundMotionsGroup);
    groundMotionsComponentsBox = new QComboBox();
    groundMotionsComponentsBox->addItem("One (Horizontal)", GroundMotionComponents::One);
    groundMotionsComponentsBox->addItem("One (Vertical)", GroundMotionComponents::Three);
    groundMotionsComponentsBox->addItem("Two (Horizontal)", GroundMotionComponents::Two);
    groundMotionsComponentsBox->addItem("Three (Horizontal & Vertical)", GroundMotionComponents::Three);

    recordsTable->setMinimumHeight(200);
    groundMotionsLayout->addWidget(recordsTable, 1, 0, 1, 2);
    groundMotionsLayout->setRowStretch(1, 1);
    progressBar = new QProgressBar();
    progressBar->setRange(0,0);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setHidden(true);
    groundMotionsLayout->addWidget(progressBar, 2, 0, 1, 2);

    // Add user-specified flatfile and input setups
    layout->addWidget(infileGroup, 0, 0, 1, 2);
    layout->addWidget(previewGroup, 1, 0, 1, 2);
    layout->addWidget(targetSpectrumGroup, 2, 0);
    layout->addWidget(recordSelectionGroup, 2, 1);
    // Output directory group location
    layout->addWidget(outdirGroup, 3, 0, 1, 2);
    layout->addWidget(scalingGroup, 4, 0, 1, 2);
    layout->addWidget(groundMotionsGroup, 5, 0, 1, 2);

    //layout->addWidget(thePlottingWindow, 0,3,2,1);
    layout->addWidget(&recordSelectionPlot, 0,3,5,1);

    recordSelectionPlot.setHidden(true);

    //layout->setRowStretch(layout->rowCount(), 1);
    layout->setRowStretch(2, 1);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(layout->columnCount(), 1);

}

void User_Defined_Database::addTableItem(int row, int column, QString value)
{
    auto item = new QTableWidgetItem(value);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    recordsTable->setItem(row, column, item);
}

void User_Defined_Database::onScalingComboBoxChanged(const int index)
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

void User_Defined_Database::setupConnections()
{
    // Output directory check
    if(outdirpath.compare("NULL") == 0)
        this->chooseOutputDirectory();

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

    connect(sfCheckBox, &QCheckBox::clicked, this, [this](bool checked){
        sfMin->setEnabled(checked);
        sfMax->setEnabled(checked);
    });

    connect(selectRecordsButton, &QPushButton::clicked, this, [this]()
    {
        // Set up the target spectrum
        setTargetSpectrum();

        // Select records
        selectRecords();
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
        connect(targetWidget, &AbstractTargetWidget::statusUpdated, this, &User_Defined_Database::updateStatus);
    }

    connect(scalingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onScalingComboBoxChanged(int)));
}

void User_Defined_Database::setRecordsTable(QList<UserSpecifiedRecord> records)
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

void User_Defined_Database::setTargetSpectrum()
{
    if(targetSpectrumDetails->currentIndex() == 0)
    {
        auto asce710widget = reinterpret_cast<ASCE710Target*>(targetSpectrumDetails->currentWidget());
        updateStatus("Computing Target Intensity Measures...");
        targetPSA = asce710widget->spectrum();
        updateStatus("Target Intensity Measures Computed.");
    }
    else
    {
        auto userTargetWidget = reinterpret_cast<AbstractTargetWidget*>(targetSpectrumDetails->currentWidget());

        progressBar->setHidden("False");
        selectRecordsButton->setEnabled(false);
        selectRecordsButton->setDown(true);

        updateStatus("Retrieving Target Intensity Measures...");
        targetPSA = userTargetWidget->spectrum();
        updateStatus("Target Intensity Measures Retrieved.");

        progressBar->setHidden(true);
        selectRecordsButton->setEnabled(true);
        selectRecordsButton->setDown(false);
    }
}

void User_Defined_Database::selectRecords()
{
    // Set the search scaling parameters
    // 0 is not scaling
    // 1 is minimize MSE
    // 2 is single period scaling
    auto scaleFlag = scalingComboBox->currentIndex();
    auto periodPoints = periodPointsLineEdit->text();
    auto weightPoints = weightsLineEdit->text();
    auto scalingPeriod = scalingPeriodLineEdit->text();
    //peerClient.setScalingParameters(scaleFlag,periodPoints,weightPoints,scalingPeriod);

    QVariant magnitudeRange;
    if(magnitudeCheckBox->checkState() == Qt::Checked)
        magnitudeRange.setValue(qMakePair(magnitudeMin->text().toDouble(), magnitudeMax->text().toDouble()));

    QVariant distanceRange;
    if(distanceCheckBox->checkState() == Qt::Checked)
        distanceRange.setValue(qMakePair(distanceMin->text().toDouble(), distanceMax->text().toDouble()));

    QVariant vs30Range;
    if(vs30CheckBox->checkState() == Qt::Checked)
        vs30Range.setValue(qMakePair(vs30Min->text().toDouble(), vs30Max->text().toDouble()));

    QVariant sfRange;
    if(sfCheckBox->checkState() == Qt::Checked)
        sfRange.setValue(qMakePair(sfMin->text().toDouble(), sfMax->text().toDouble()));

    // Conditional period (if specified)
    double T_cond;
    if (scaleFlag == 2)
        T_cond = scalingPeriod.toDouble();

    // Set up period points and weights
    QVector<double> refPeriods;
    QVector<double> refWeights;
    QVector<double> tmpT;
    QVector<double> tmpW;
    updateStatus("Selecting records...");
    if (scaleFlag == 0)
    {
        // No scaling, using the target spectrum periods as the reference
        for (int i = 0; i != targetPSA.length(); ++i)
        {
            refPeriods << targetPSA[i].first;
            refWeights << 1.0;
        }
    }
    else
    {
        // Use the scaling periods as the reference
        auto tmpPeriods = periodPoints.split(",");
        auto tmpWeights = weightPoints.split(",");
        for (int i = 0; i != tmpPeriods.size(); ++i)
        {
            tmpT << tmpPeriods.at(i).toDouble();
            tmpW << tmpWeights.at(i).toDouble();
        }
        // We now need to interpolate the weights
        for (int i = 0; i != targetPSA.length(); ++i)
        {
            if (targetPSA[i].first >= tmpT[0] && targetPSA[i].first <= tmpT[tmpT.length()-1])
            {
                refPeriods << targetPSA[i].first;
                refWeights << interpolateSpectrum(tmpT, tmpW, targetPSA[i].first);
            }
        }
    }

    // Interpolate the target spectrum given the refPeriods
    QVector<double> refSpectrum;
    if (scaleFlag > 0)
    {
        QVector<double> tData;
        QVector<double> saData;
        for (int i = 0; i != targetPSA.length(); ++i)
        {
            tData << targetPSA[i].first;
            saData << targetPSA[i].second;
        }
        for (int i = 0; i != refPeriods.length(); ++i)
            refSpectrum << interpolateSpectrum(tData, saData, refPeriods[i]);
    }
    else
    {
        for (int i = 0; i != targetPSA.length(); ++i)
            refSpectrum << targetPSA[i].second;
    }

    // Start select ground motion
    QList<double> cadErr;
    QList<double> cadScale;
    for (int i = 0; i != allRecords.length(); ++i)
    {
        auto curRecord = allRecords[i];
        // Check if earthquake characteristics fulfill user-defined ranges

        if (magnitudeCheckBox->checkState() == Qt::Checked && \
                (curRecord.Magnitude < magnitudeMin->text().toDouble() || \
                curRecord.Magnitude > magnitudeMax->text().toDouble()))
        {
            cadErr.append(100000.0);
            cadScale.append(100000.0);
            continue;
        }
        if (vs30CheckBox->checkState() == Qt::Checked && \
                (curRecord.Vs30 < vs30Min->text().toDouble() || \
                curRecord.Vs30 > vs30Max->text().toDouble()))
        {
            cadErr.append(100000.0);
            cadScale.append(100000.0);
            continue;
        }
        if (distanceCheckBox->checkState() == Qt::Checked && \
                (curRecord.Distance < distanceMin->text().toDouble() || \
                curRecord.Distance > distanceMax->text().toDouble()))
        {
            cadErr.append(100000.0);
            cadScale.append(100000.0);
            continue;
        }

        // Interpolate the current spectrum first
        QVector<double> curSpectrum;
        for (int j = 0; j != refPeriods.length(); ++j)
        {
            curSpectrum << interpolateSpectrum(recordPeriods, curRecord.Spectrum, refPeriods[j]);
        }

        // Compute the min error and corresponding scaling factor for the current record
        double recordScale;
        double minErr = 10000000.0;
        if (scaleFlag == 0)
        {
            // No scaling
            recordScale = 1.0;
            minErr = 0.0;
            for (int j = 0; j != refPeriods.length(); ++j)
                minErr = minErr + pow(refSpectrum[j] - recordScale*curSpectrum[j], 2.0);
        }
        else if (scaleFlag == 1)
        {
            double sf_min;
            double sf_max;
            if (sfCheckBox->checkState() == Qt::Checked)
            {
                sf_min = sfMin->text().toDouble();
                sf_max = sfMax->text().toDouble();
            }
            else
            {
                // Compute the min and max scaling factors for minizing the error
                // Not here we compute the scaling factor to pin the tmpSpectrum and refSpecrum at each of
                // the refPeriods to find out the min and max
                sf_min = 1000000.0;
                sf_max = 0.0;
                for (int j = 0; j != refPeriods.length(); ++j)
                {
                    if (refSpectrum[j]/curSpectrum[j] <= sf_min)
                        sf_min = refSpectrum[j]/curSpectrum[j];
                    if (refSpectrum[j]/curSpectrum[j] >= sf_max)
                        sf_max = refSpectrum[j]/curSpectrum[j];
                }
            }
            // Assuming we try to scale the record with 20 scaling factors from sf_min to sf_max
            // Computing the spectral matching error for each scaling factor and find the min error
            double sf_delta = (sf_max - sf_min) / 19;
            double sf_cur = sf_min;
            while (sf_cur <= sf_max)
            {
                double tmpErr = 0.0;
                for (int j = 0; j != refPeriods.length(); ++j)
                    tmpErr = tmpErr + pow(refSpectrum[j] - sf_cur*curSpectrum[j], 2.0);
                if (minErr > tmpErr)
                {
                    minErr = tmpErr;
                    recordScale = sf_cur;
                }
                sf_cur = sf_cur + sf_delta;
            }
        }
        else
        {
            double sf_min = 0.0;
            double sf_max = 100000.0;
            if (sfCheckBox->checkState() == Qt::Checked)
            {
                sf_min = sfMin->text().toDouble();
                sf_max = sfMax->text().toDouble();
            }
            // Pin at the conditional period
            recordScale = interpolateSpectrum(refPeriods, refSpectrum, T_cond) / interpolateSpectrum(refPeriods, curSpectrum, T_cond);
            if (recordScale < sf_min || recordScale > sf_max)
            {
                cadErr.append(100000.0);
                cadScale.append(100000.0);
                continue;
            }
            minErr = 0.0;
            for (int j = 0; j != refPeriods.length(); ++j)
                minErr = minErr + pow(refSpectrum[j] - recordScale*curSpectrum[j], 2.0);
        }
        // Collect
        cadErr.append(minErr);
        cadScale.append(recordScale);
    }

    // Prepare records
    int nRecords = nRecordsEditBox->text().toInt();
    currentRecords.clear();
    currentRecords.reserve(nRecords);
    auto cadErrSort = cadErr;
    qStableSort(cadErrSort.begin(), cadErrSort.end());
    QList<int> recordTag;
    for (int i = 0; i != nRecords; ++i)
    {
        auto j = qFind(cadErr.begin(), cadErr.end(), cadErrSort[i]) - cadErr.begin();
        UserSpecifiedRecord record;
        record.RSN = allRecords[j].RSN;
        record.Earthquake = allRecords[j].Earthquake;
        record.Station = allRecords[j].Station;
        record.Magnitude = allRecords[j].Magnitude;
        record.Distance = allRecords[j].Distance;
        record.Vs30 = allRecords[j].Vs30;
        record.Horizontal1File = allRecords[j].Horizontal1File;
        record.Horizontal2File = allRecords[j].Horizontal2File;
        record.VerticalFile = allRecords[j].VerticalFile;
        record.Spectrum = allRecords[j].Spectrum;
        record.Scale = cadScale[j];

        currentRecords.push_back(record);
    }

    clearSpectra();

    // Set up records table
    setRecordsTable(currentRecords);

    // Plot spectra
    plotSpectra();

    updateStatus("Records selected.");
    updateStatus("Saving results...");

    saveResultsToFolder();

    updateStatus("Results saved in \"SelectResults.csv\" in the directory " + outdirpath + ".");

    //GoogleAnalytics::Report("RecordSelection", "PEER");

}

void User_Defined_Database::updateStatus(QString status)
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

void User_Defined_Database::plotSpectra()
{
    //Spectra can be plotted here using the data in
    //periods, targetSpectrum, meanSpectrum, meanPlusSigmaSpectrum, meanMinusSigmaSpectrum, scaledSelectedSpectra

    for (int i = 0; i != nRecordsEditBox->text().toInt(); ++i)
    {
        QVector<double> tmpSa;
        for (int j = 0; j != currentRecords[i].Spectrum.length(); ++j)
        {
            tmpSa << currentRecords[i].Spectrum[j] * currentRecords[i].Scale;
        }
        scaledSelectedSpectra << tmpSa;
    }

    QVector<double> sumSa;
    for (int j = 0; j != currentRecords[0].Spectrum.length(); ++j)
    {
        double tmpSa = 0.0;
        for (int i = 0; i != nRecordsEditBox->text().toInt(); ++i)
        {
            tmpSa = tmpSa + currentRecords[i].Spectrum[j] * currentRecords[i].Scale;
        }
        sumSa << tmpSa;
    }
    for (int j = 0; j != currentRecords[0].Spectrum.length(); ++j)
        meanSpectrum << sumSa[j]/nRecordsEditBox->text().toDouble();


    QVector<double> tData;
    QVector<double> saData;
    for (int i = 0; i != targetPSA.length(); ++i)
    {
        tData << targetPSA[i].first;
        saData << targetPSA[i].second;
    }

    recordSelectionPlot.setHidden(false);
    recordSelectionPlot.setSelectedSpectra(recordPeriods, scaledSelectedSpectra);
    recordSelectionPlot.setMean(recordPeriods, meanSpectrum);
    recordSelectionPlot.setMeanPlusSigma(periods, meanPlusSigmaSpectrum);
    recordSelectionPlot.setMeanMinusSigma(periods, meanMinusSigmaSpectrum);
    recordSelectionPlot.setTargetSpectrum(tData, saData);

    auto size = recordSelectionPlot.size();
    size.setWidth(size.height());
    recordSelectionPlot.setMinimumSize(size);
}

void User_Defined_Database::previewSpectra()
{
    //Spectra can be plotted here using the data in
    // Preview number of ground motions
    int preview_size;
    if (currentRecords.length() > 1000)
        preview_size = 1000;
    else
        preview_size = currentRecords.length();
    for (int i = 0; i != preview_size; ++i)
    {
        QVector<double> tmpSa;
        for (int j = 0; j != currentRecords[i].Spectrum.length(); ++j)
        {
            tmpSa << currentRecords[i].Spectrum[j] * currentRecords[i].Scale;
        }
        scaledSelectedSpectra << tmpSa;
    }

    QVector<double> sumSa;
    for (int j = 0; j != currentRecords[0].Spectrum.length(); ++j)
    {
        double tmpSa = 0.0;
        for (int i = 0; i != nRecordsEditBox->text().toInt(); ++i)
        {
            tmpSa = tmpSa + currentRecords[i].Spectrum[j] * currentRecords[i].Scale;
        }
        sumSa << tmpSa;
    }
    for (int j = 0; j != currentRecords[0].Spectrum.length(); ++j)
        meanSpectrum << sumSa[j]/nRecordsEditBox->text().toDouble();

    recordSelectionPlot.setHidden(false);
    recordSelectionPlot.setSelectedSpectra(recordPeriods, scaledSelectedSpectra);
    recordSelectionPlot.setMean(recordPeriods, meanSpectrum);
    recordSelectionPlot.setMeanPlusSigma(periods, meanPlusSigmaSpectrum);
    recordSelectionPlot.setMeanMinusSigma(periods, meanMinusSigmaSpectrum);

    auto size = recordSelectionPlot.size();
    size.setWidth(size.height());
    recordSelectionPlot.setMinimumSize(size);
}

void User_Defined_Database::clearSpectra()
{
    periods.clear();
    meanSpectrum.clear();
    meanPlusSigmaSpectrum.clear();
    meanMinusSigmaSpectrum.clear();
    targetSpectrum.clear();
    scaledSelectedSpectra.clear();
}

void User_Defined_Database::setOutputDirectory(QString dirpath) {
    outdirLE->setText(dirpath);
    return;
}

void User_Defined_Database::chooseOutputDirectory(void) {
    outdirpath=QFileDialog::getExistingDirectory(this,tr("Output Folder"));
    if(outdirpath.isEmpty())
    {
        outdirpath = "NULL";
        return;
    }
    this->setOutputDirectory(outdirpath);

}

void User_Defined_Database::setGMDirectory(QString dirpath) {
    gmDataLE->setText(dirpath);
    return;
}

void User_Defined_Database::chooseGM(void) {
    gmpath=QFileDialog::getExistingDirectory(this,tr("Ground Motion Folder"));
    if(gmpath.isEmpty())
    {
        gmpath = "NULL";
        return;
    }
    this->setGMDirectory(gmpath);

}

void User_Defined_Database::setFlatfile(QString dirpath) {
    flatfileLE->setText(dirpath);
    return;
}

void User_Defined_Database::chooseFlatfile(void) {
    flatfilepath=QFileDialog::getOpenFileName(this,tr("Flatfile"));
    if(flatfilepath.isEmpty())
    {
        flatfilepath = "NULL";
        return;
    }
    this->setFlatfile(flatfilepath);
    this->loadFlatfile();
}

void User_Defined_Database::loadFlatfile(void)
{
    updateStatus("Loading flatfile...");

    // If the file path is not found, jump to chooseFlatfile
    if (flatfilepath.compare("Null") == 0)
        this->chooseFlatfile();
    // If still empty, return
    if (flatfilepath.compare("Null") == 0)
        return;

    // Try to load the file
    auto dFlag = this->parseFlatfile(flatfilepath);
    if (dFlag != 0)
    {
        QString errMsg = "File: " + flatfilepath + " cannot be parsed.";
        emit sendStatusMessage(errMsg);
        return;
    }

    updateStatus("Flatfile loaded and can be previewed (atmost 1000 records).");

    // plot spectra
    currentRecords = allRecords;
    clearSpectra();
    previewSpectra();
}

int User_Defined_Database::parseFlatfile(const QString& pathToFile)
{
    // tmpData is a QVector with QStringLists
    // Each QStringList is for one record
    // With divider ",", QStringList has multiple attributes
    auto tmpData = parseCSVFile(pathToFile);

    // Empty check
    if (tmpData.empty())
        return -1;

    auto colName = tmpData[0];
    // Find the required column names and their corresponding indices
    QMap<QString, int>::iterator i;
    for (i = attributeIndex.begin(); i != attributeIndex.end(); ++i)
    {
        for (int j = 0; j != colName.size(); ++j)
        {
            if (i.key().compare(colName[j]) == 0)
                attributeIndex[i.key()] = j;
        }
    }
    // Find the indices of period points
    QVector<int> periodIndex;
    for (int j = 0; j != colName.size(); j++)
    {
        if (colName[j].startsWith("T") && colName[j].endsWith("S"))
            periodIndex << j;
    }

    // Parse data for records
    allRecords.reserve(tmpData.length());
    for (int rtag = 1; rtag != tmpData.length(); rtag++)
    {
        auto curRecord = tmpData[rtag];
        UserSpecifiedRecord record;
        record.RSN = curRecord[attributeIndex["RSN"]].toInt();
        record.Earthquake = curRecord[attributeIndex["Earthquake Name"]];
        record.Station = curRecord[attributeIndex["Station"]];
        record.Magnitude = curRecord[attributeIndex["Magnitude"]].toDouble();
        record.Distance = curRecord[attributeIndex["Distance"]].toDouble();
        record.Vs30 = curRecord[attributeIndex["Vs30"]].toDouble();
        record.Horizontal1File = curRecord[attributeIndex["Filename (H1)"]];
        record.Horizontal2File = curRecord[attributeIndex["Filename (H2)"]];
        record.VerticalFile = curRecord[attributeIndex["Filename (V)"]];
        record.Scale = 1.0;
        for (int ttag = periodIndex[0]; ttag != periodIndex[periodIndex.length()-1]; ttag++)
            record.Spectrum << curRecord[ttag].toDouble();

        allRecords.push_back(record);
    }

    // update the table
    tmpData.pop_front();
    this->updateFlatfileSpreadSheet(tmpData);

    return 0;
}

QVector<QStringList> User_Defined_Database::parseCSVFile(const QString &pathToFile)
{
    QVector<QStringList> returnVec;

    QFile tmpFile(pathToFile);

    if (!tmpFile.open(QIODevice::ReadOnly))
    {
        QString errMsg = "Cannot find the file: " + pathToFile + "\nCheck your directory and try again.";
        emit sendStatusMessage(errMsg);
        return returnVec;
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
        qDebug()<<"Error in parsing the .csv file "<<pathToFile <<" in UserDefinedDatabase::parseCSV file";
        return returnVec;
    }

    returnVec.reserve(numRows);

    for(auto&& it: rowLines)
    {
        auto lineStr = this->parseLineCSV(it);

        returnVec.push_back(lineStr);
    }

    return returnVec;
}

QStringList User_Defined_Database::parseLineCSV(const QString &csvString)
{
    QStringList fields;
    QString value;

    bool hasQuote = false;

    for (int i = 0; i < csvString.size(); ++i)
    {
        const QChar current = csvString.at(i);

        // Normal state
        if (hasQuote == false)
        {
            // Comma
            if (current == ',')
            {
                // Save field
                fields.append(value.trimmed());
                value.clear();
            }

            // Double-quote
            else if (current == '"')
            {
                hasQuote = true;
                value += current;
            }

            // Other character
            else
                value += current;
        }
        else if (hasQuote)
        {
            // Check for another double-quote
            if (current == '"')
            {
                if (i < csvString.size())
                {
                    // A double double-quote?
                    if (i+1 < csvString.size() && csvString.at(i+1) == '"')
                    {
                        value += '"';

                        // Skip a second quote character in a row
                        i++;
                    }
                    else
                    {
                        hasQuote = false;
                        value += '"';
                    }
                }
            }

            // Other character
            else
                value += current;
        }
    }

    if (!value.isEmpty())
        fields.append(value.trimmed());


    // Remove quotes and whitespace around quotes
    for (int i=0; i<fields.size(); ++i)
        if (fields[i].length()>=1 && fields[i].left(1)=='"')
        {
            fields[i]=fields[i].mid(1);
            if (fields[i].length()>=1 && fields[i].right(1)=='"')
                fields[i]=fields[i].left(fields[i].length()-1);
        }

    return fields;
}

void User_Defined_Database::onSpreadsheetCellClicked(int row, int column)
{

    // Check to make sure that a table widget is sending this
    QTableWidget* tableSender = qobject_cast<QTableWidget*>(sender());
    if(tableSender == nullptr)
        return;

    QTableWidgetItem *item = tableSender->item(row,column);
    cellText = item->text();

}


void User_Defined_Database::onSpreadsheetCellChanged(int row, int column)
{

    // Check to make sure that a table widget is sending this
    QTableWidget* tableSender = qobject_cast<QTableWidget*>(sender());
    if(tableSender == nullptr || tableSender == flatfileSpreadSheet)
        return;

    if (updatingFlatfileTable == false)
    {
        QTableWidgetItem *item = tableSender->item(row,column);
        QString newText = item->text();
    }
}

void User_Defined_Database::setDefaultValues(void)
{
    defaultCol << "RSN" << "Earthquake Name" << "Station" << "Magnitude" << "Distance" << \
                  "Vs30" << "Filename (H1)" << "Filename (H2)" << "Filename (V)";
    // required column attributes and default indices
    for (int i = 0; i != defaultCol.length(); ++i)
        attributeIndex.insert(defaultCol[i], i);

    // Records' period points (using the PEER standard)
    recordPeriods << 0.01 << 0.02 << 0.022 << 0.025 << 0.029 << 0.030 << 0.032 << 0.035 \
                  << 0.036 << 0.04 << 0.042 << 0.044 << 0.045 << 0.046 << 0.048 << 0.05 \
                  << 0.055 << 0.06 << 0.065 << 0.067 << 0.07 << 0.08 << 0.09 << 0.1 << 0.11 \
                  << 0.12 << 0.13 << 0.133 << 0.14 << 0.15 << 0.16 << 0.17 << 0.18 << 0.19 \
                  << 0.2 << 0.22 << 0.24 << 0.25 << 0.26 << 0.28 << 0.29 << 0.3 << 0.32 \
                  << 0.34 << 0.35 << 0.36 << 0.38 << 0.4 << 0.42 << 0.44 << 0.45 << 0.46 \
                  << 0.48 << 0.5 << 0.55 << 0.6 << 0.65 << 0.667 << 0.7 << 0.75 << 0.8 \
                  << 0.85 << 0.9 << 0.95 << 1.0 << 1.1 << 1.2 << 1.3 << 1.4 << 1.5 \
                  << 1.6 << 1.7 << 1.8 << 1.9 << 2.0 << 2.2 << 2.4 << 2.5 << 2.6 << 2.8 \
                  << 3.0 << 3.2 << 3.4 << 3.5 << 3.6 << 3.8 << 4.0 << 4.2 << 4.4 << 4.6 \
                  << 4.8 << 5.0 << 5.5 << 6.0 << 6.5 << 7.0 << 7.5 << 8.0 << 8.5 << 9.0 \
                  << 9.5 << 10.0 << 11.0 << 12.0 << 13.0 << 14.0 << 15.0 << 20.0;
}

void User_Defined_Database::updateFlatfileSpreadSheet(const QVector<QStringList>& data)
{

    if(data.empty())
    {
        qDebug()<<"Error in UserDefinedDatabase::updateFlatFile. Empty data vector";
        return;
    }

    flatfileSpreadSheet->clear();
    flatfileSpreadSheet->setColumnCount(9);
    flatfileSpreadSheet->setRowCount(data.size());
    QStringList tableHeadings;
    foreach (const QString str, defaultCol)
        tableHeadings << str;
    flatfileSpreadSheet->setHorizontalHeaderLabels(tableHeadings);
    //flatfileSpreadSheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    flatfileSpreadSheet->verticalHeader()->setVisible(false);

    updatingFlatfileTable = true;
    // Preview 1000 records at most
    int preview_size;
    if (data.size() > 1000)
        preview_size = 1000;
    else
        preview_size = data.size();
    for (int i = 0; i< preview_size; ++i)
    {
        auto tableRow = data[i];

        QTableWidgetItem *item;
        for (int j = 0; j != 9; ++j)
        {
            item = new QTableWidgetItem(tableRow[j]);
            flatfileSpreadSheet->setItem(i, j, item);
        }
    }

    updatingFlatfileTable = false;

    return;
}

void User_Defined_Database::saveResultsToFolder(void)
{

    // Ask for the file path if the file path has not yet been set
    if(outdirpath.compare("NULL") == 0)
        this->chooseOutputDirectory();

    if(outdirpath.compare("NULL") == 0)
        return;

    // Check if the directory exists
    QDir directory(outdirpath);
    if (!directory.exists())
    {
        QString errMsg = "Cannot find the directory: "+ outdirpath + "\n" +"Check your directory and try again.";
        emit sendStatusMessage(errMsg);
        return;
    }

    auto dirToResults = directory.filePath("SelectResults.csv");

    auto res = this->saveSelectResults(dirToResults);
    if(res != 0)
    {
        QString errMsg = "Error saving the SelectResults.csv file in the directory " + outdirpath + "\n" +"Check the file and try again.";
        emit sendStatusMessage(errMsg);
        return;
    }

    return;
}

int User_Defined_Database::saveSelectResults(const QString& pathToFile)
{
    QFile file(pathToFile);

    if (!file.open(QIODevice::WriteOnly))
    {
        QString errMsg = "Cannot find the required file: SelectResults.csv in the directory" + outdirpath + "\n" +"Check your directory and try again.";
        emit sendStatusMessage(errMsg);
        return -1;
    }

    QTextStream csvFileOut(&file);

    QString headerLine = "RSN,Scale,Earthquake,Station,Magnitude,Distance,Vs30,Horizontal 1 File,Horizontal 2 File,Vertical File\n";

    csvFileOut<<headerLine.toUtf8();

    auto numRows = recordsTable->rowCount();
    auto numCols = recordsTable->columnCount();

    for (int i = 0; i< numRows; ++i)
    {
        for(int j = 0; j<numCols; ++j)
        {
            if (recordsTable->item(i,j)->text().contains(","))
                csvFileOut<< "\"" + recordsTable->item(i,j)->text() + "\"";
            else
                csvFileOut<<recordsTable->item(i,j)->text();

            // Add the terminating character
            if(j != numCols-1)
                csvFileOut<<",";
            else
                csvFileOut<<"\n";
        }
    }

    return 0;
}

double User_Defined_Database::interpolateSpectrum(QVector<double> xData, QVector<double> yData, double x)
{
    // Exceed the right boundary
    if (x >= xData[xData.size()-1])
        return yData[yData.size()-1];

    // Exceed the left boundary
    if (x <= xData[0])
        return yData[0];

    // Interpolation if within the range
    int i = 0;
    if (x >= xData[xData.size()-2])
        i = xData.size()-2;
    else
    {
        while (x > xData[i+1])
            i = i+1;
    }
    double xL = xData[i];
    double yL = yData[i];
    double xR = xData[i+1];
    double yR = yData[i+1];
    double k = (yR - yL) / (xR - xL);
    return yL + k * (x - xL);
}








