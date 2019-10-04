#ifndef PEERNGARECORDSWIDGET_H
#define PEERNGARECORDSWIDGET_H

#include <QWidget>
#include <SimCenterAppWidget.h>
#include <QPushButton>
#include "PeerNgaWest2Client.h"
#include <QLineEdit>
#include <QDir>
#include <QFile>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QVector>
#include <QCheckBox>
#include "RecordSelectionPlot.h"


struct PeerScaledRecord
{
    int RSN;
    QString Station;
    QString Earthquake;
    QString Horizontal1File;
    QString Horizontal2File;
    QString VerticalFile;
    double scale;
};

class PeerNgaRecordsWidget : public SimCenterAppWidget
{
    Q_OBJECT


public:
    explicit PeerNgaRecordsWidget(QWidget *parent = nullptr);

    bool outputToJSON(QJsonObject &jsonObject) override;
    bool inputFromJSON(QJsonObject &jsonObject) override;

    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &destDir) override;

    enum GroundMotionComponents{One, Two, Three};
    Q_ENUM(GroundMotionComponents)

signals:

public slots:

private:
    PeerNgaWest2Client peerClient;
    QPushButton* selectRecordsButton;
    QLineEdit* sdsEditBox;
    QLineEdit* sd1EditBox;
    QLineEdit* tlEditBox;
    QLineEdit* nRecordsEditBox;
    QTableWidget* recordsTable;
    QComboBox* groundMotionsComponentsBox;
    RecordSelectionPlot recordSelectionPlot;

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

    //Record Selection Members
    QList<PeerScaledRecord> currentRecords;
    QVector<QVector<double>> scaledSelectedSpectra;
    QVector<double> periods;
    QVector<double> meanSpectrum;
    QVector<double> meanPlusSigmaSpectrum;
    QVector<double> meanMinusSigmaSpectrum;
    QVector<double> targetSpectrum;

    void setupUI();
    void setupConnections();
    void processPeerRecords(QDir resultFolder);
    QList<PeerScaledRecord> parseSearchResults(QString searchResultsFilePath);
    void setRecordsTable(QList<PeerScaledRecord>);
    void clearSpectra();
    void plotSpectra();
    void updateStatus(QString status);
    void selectRecords();

};

#endif // PEERNGARECORDSWIDGET_H
