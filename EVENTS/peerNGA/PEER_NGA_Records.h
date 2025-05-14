#ifndef PEER_NGA_RECORDS_H
#define PEER_NGA_RECORDS_H

#include <QWidget>
#include <SimCenterAppWidget.h>
#include <QPushButton>
#include "PeerNgaWest2Client.h"
#include <QLineEdit>
#include <QDir>
#include <QFile>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QPointer>
#include <QVector>
#include <QCheckBox>
#include "RecordSelectionPlot.h"
#include <QProgressBar>
#include <QStackedWidget>
#include <GeneralInformationWidget.h>
#include <QWebEngineView>


class QComboBox;
class QLabel;
class QGroupBox;
class QTabWidget;
class SpectrumFromRegionalSurrogate;
class UserSpectrumWidget;

struct PeerScaledRecord
{
    int RSN;
    QString Station;
    QString Earthquake;
    QString Horizontal1File;
    QString Horizontal2File;
    QString VerticalFile;
    double Scale;
    double Magnitude;
    double Distance;
    double Vs30;
    QJsonObject metadata;
};

class PEER_NGA_Records : public SimCenterAppWidget
{
    Q_OBJECT


public:
    explicit PEER_NGA_Records(GeneralInformationWidget* generalInfoWidget, QWidget *parent = nullptr);
    ~PEER_NGA_Records();

    bool outputToJSON(QJsonObject &jsonObject) override;
    bool inputFromJSON(QJsonObject &jsonObject) override;

    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &destDir) override;

    enum GroundMotionComponents{One, Two, Three};
    Q_ENUM(GroundMotionComponents)

    void setOutputDirectory(QString filename);    
    bool outputCitation(QJsonObject &jsonObject) override;

signals:

public slots:

    void onScalingComboBoxChanged(const int index);
    void chooseOutputDirectory(void);
    void switchUserDefined(QString dirName, QString fileName);

private:

    void downloadRecordBatch();

    // widget groups
    QGroupBox *recordSelectionGroup;
    QGridLayout *targetSpectrumLayout;
    PeerNgaWest2Client peerClient;
    QPushButton* selectRecordsButton;
    QLineEdit* nRecordsEditBox;
    QTableWidget* recordsTable;
    QComboBox* groundMotionsComponentsBox;
    RecordSelectionPlot recordSelectionPlot;
    QProgressBar* progressBar;
    QComboBox* spectrumTypeComboBox;
    QStackedWidget* targetSpectrumDetails;
    QGridLayout* recordSelectionLayout;
    QComboBox* suiteAverageBox; // how to compute the suite average
    QComboBox* faultTypeBox; // fault type in PEER record searching
    QComboBox* pulseBox; // whether filtering records by pulse-like feature

    //Magnitude Range
    QCheckBox* magnitudeCheckBox;
    QLineEdit* magnitudeMin;
    QLineEdit* magnitudeMax;

    //Distance Range
    QCheckBox* distanceCheckBox;
    QLineEdit* distanceMin;
    QLineEdit* distanceMax;

    //Vs30 Range
    QCheckBox* vs30CheckBox;
    QLineEdit* vs30Min;
    QLineEdit* vs30Max;
    QDir *groundMotionsFolder;

    //D5-95 Range
    QCheckBox* durationCheckBox;
    QLineEdit* durationMin;
    QLineEdit* durationMax;

    //Scaling
    QComboBox* scalingComboBox;
    QLabel* scalingPeriodLabel1;
    QLineEdit* scalingPeriodLineEdit;
    QLabel* scalingPeriodLabel2;

    //Weight function inputs
    QPointer<QLabel> weightFunctionHeadingLabel;
    QPointer<QLabel> weightFunctionLabel;
    QPointer<QLabel> periodPointsLabel1;
    QPointer<QLineEdit> periodPointsLineEdit;
    QPointer<QLabel> periodPointsLabel2;
    QPointer<QLabel> weightsLabel1;
    QPointer<QLineEdit> weightsLineEdit;
    QPointer<QLabel> weightsLabel2;

    //Record Selection Members
    QList<PeerScaledRecord> currentRecords;
    QVector<QVector<double>> scaledSelectedSpectra;
    QVector<double> periods;
    QVector<double> meanSpectrum;
    QVector<double> meanPlusSigmaSpectrum;
    QVector<double> meanMinusSigmaSpectrum;
    QVector<double> targetSpectrum;

    //Display of coverage image
    //QLabel *coverageImage;
    QWebEngineView * coverageImage;


    // Output directory
    QLineEdit* outdirLE;
    QString outdirpath;
    QString RecordsDir;

    // Additional scaling
    QVector<double> additionalScaling;
    QStringList RSN;
    int numDownloaded;
    void setupUI(GeneralInformationWidget* generalInfoWidget);
    void setupConnections();
    void processPeerRecords(QDir resultFolder);
    QList<PeerScaledRecord> parseSearchResults(QString searchResultsFilePath);
    void setRecordsTable(QList<PeerScaledRecord>);
    void clearSpectra();
    void plotSpectra();
    void updateStatus(QString status);
    void selectRecords();
    void addTableItem(int row, int Column, QString value);

    SpectrumFromRegionalSurrogate *spectrumSurrogate;
    UserSpectrumWidget *userSpectrumTarget;

    QGridLayout *layout;
  QTabWidget *theTabWidget;

};

#endif // PEER_NGA_RECORDS_H
