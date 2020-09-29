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


class QComboBox;
class QLabel;
class QGroupBox;

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

};

class PEER_NGA_Records : public SimCenterAppWidget
{
    Q_OBJECT


public:
    explicit PEER_NGA_Records(GeneralInformationWidget* generalInfoWidget, QWidget *parent = nullptr);

    bool outputToJSON(QJsonObject &jsonObject) override;
    bool inputFromJSON(QJsonObject &jsonObject) override;

    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &destDir) override;

    enum GroundMotionComponents{One, Two, Three};
    Q_ENUM(GroundMotionComponents)

signals:

public slots:

    void onScalingComboBoxChanged(const int index);

private:
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

};

#endif // PEER_NGA_RECORDS_H
