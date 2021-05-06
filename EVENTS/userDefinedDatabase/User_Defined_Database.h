#ifndef USER_DEFINED_DATABASE_H
#define USER_DEFINED_DATABASE_H

#include <QWidget>
#include <SimCenterAppWidget.h>
#include <QPushButton>
//#include "PeerNgaWest2Client.h"
#include <QLineEdit>
#include <QDir>
#include <QFile>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QPointer>
#include <QVector>
#include <QCheckBox>
#include "../peerNGA/RecordSelectionPlot.h"
#include <QProgressBar>
#include <QStackedWidget>
#include <GeneralInformationWidget.h>


class QComboBox;
class QLabel;
class QGroupBox;

struct UserSpecifiedRecord
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
    QVector<double> Spectrum;
};

class User_Defined_Database : public SimCenterAppWidget
{
    Q_OBJECT


public:
    explicit User_Defined_Database(GeneralInformationWidget* generalInfoWidget, QWidget *parent = nullptr);

    //bool outputToJSON(QJsonObject &jsonObject) override;
    //bool inputFromJSON(QJsonObject &jsonObject) override;

    //bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    //bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    //bool copyFiles(QString &destDir) override;

    enum GroundMotionComponents{One, Two, Three};
    Q_ENUM(GroundMotionComponents)

    void setOutputDirectory(QString filename);
    void setGMDirectory(QString filename);
    void setFlatfile(QString filename);

    // Load user-specified ground motion flatfile
    void loadFlatfile(void);

signals:

public slots:

    void onScalingComboBoxChanged(const int index);
    void chooseOutputDirectory(void);
    void chooseFlatfile(void);
    void chooseGM(void);

    // Table editing slots
    void onSpreadsheetCellChanged(int row, int column);
    void onSpreadsheetCellClicked(int row, int column);

private:
    //PeerNgaWest2Client peerClient;
    QPushButton* selectRecordsButton;
    QLineEdit* nRecordsEditBox;
    QTableWidget* recordsTable;
    QComboBox* groundMotionsComponentsBox;
    RecordSelectionPlot recordSelectionPlot;
    QProgressBar* progressBar;
    QComboBox* spectrumTypeComboBox;
    QStackedWidget* targetSpectrumDetails;
    QGridLayout* recordSelectionLayout;

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
    QTemporaryDir groundMotionsFolder;

    //Scaling Range
    QCheckBox* sfCheckBox;
    QLineEdit* sfMin;
    QLineEdit* sfMax;

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
    QList<UserSpecifiedRecord> allRecords;
    QList<UserSpecifiedRecord> currentRecords;
    QVector<QVector<double>> scaledSelectedSpectra;
    QVector<double> periods;
    QVector<double> meanSpectrum;
    QVector<double> meanPlusSigmaSpectrum;
    QVector<double> meanMinusSigmaSpectrum;
    QVector<double> targetSpectrum;

    // Output directory
    QLineEdit* outdirLE;
    QString outdirpath;
    QString RecordsDir;

    // GM database
    QLineEdit* flatfileLE;
    QLineEdit* gmDataLE;
    QString gmpath;
    QString flatfilepath;
    QString cellText;
    bool updatingFlatfileTable;

    void setupUI(GeneralInformationWidget* generalInfoWidget);
    void setupConnections();
    //void processPeerRecords(QDir resultFolder);
    //QList<UserSpecifiedRecord> parseSearchResults(QString searchResultsFilePath);
    void setRecordsTable(QList<UserSpecifiedRecord>);
    void clearSpectra();
    void plotSpectra();
    void previewSpectra();
    void updateStatus(QString status);
    void setTargetSpectrum();
    void selectRecords();
    void addTableItem(int row, int Column, QString value);

    // Functions to import ground motion flatfile from the csv file
    int parseFlatfile(const QString& pathToFile);

    // Functions to parse csv files
    QVector<QStringList> parseCSVFile(const QString &string);
    QStringList parseLineCSV(const QString &string);

    // Flatfile table widget
    QVector<double> recordPeriods; // the default period points in the csv, fixed now.
    QVector<QString> defaultCol;
    QTableWidget *flatfileSpreadSheet;
    void updateFlatfileSpreadSheet(const QVector<QStringList>& data);

    // Ground motion flatfile attributes
    QMap<QString, int> attributeIndex;

    // Set default values
    void setDefaultValues();

    // Target spectrum
    QList<QPair<double,double>> targetPSA;

    // Interpolate spectrum
    double interpolateSpectrum(QVector<double> xData, QVector<double> yData, double x);

    // Save selection results
    void saveResultsToFolder();
    int saveSelectResults(const QString& pathToFile);
};

#endif // USER_DEFINED_DATABASE
