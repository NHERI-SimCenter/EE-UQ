#ifndef SPECTRUMFROMREGIONALSURROGATE_H
#define SPECTRUMFROMREGIONALSURROGATE_H

#include "AbstractTargetWidget.h"
#include "UserSpectrumModel.h"
#include <QFile>
#include <QDir>
#include <QTableView>
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QVector>
#include <QProcess>
#include "RandomVariablesContainer.h"

class QRegExpValidator;

class SpectrumFromRegionalSurrogate : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit SpectrumFromRegionalSurrogate(QWidget *parent = nullptr);
    void setMainScript(QString filnema1); // synced from surrogateGpFEM
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;
    //bool inputFromJSON(QJsonObject &jsonObject);
    //bool outputToJSON(QJsonObject &jsonObject);
    bool writeConfigJSON(QJsonObject &configObject);
    QList<QPair<double, double> > spectrum() const override;
    bool copyFiles(QString dirName);
    static bool copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory);

public slots:
    void loadCSV(void);
    void loadSpectrum(void);
    void updateMessage(QString);
    void showGpOptions(QString);
    void showRVs(QStringList &varNamesAndValues);
    void getSpectrum(void);
    QString checkPeriodsValid(const QString& input) const;
    void commitPeriods();
    void saveSpectrum(bool runFlag, QString dirName, QString fileName);

signals:
    void runComplete(bool, QString, QString);
    void spectrumSaved(QString, QString);

private:
    void specialCopyMainScript(QString fileName, QStringList varNamesAndValues);
    double interpolateForGP(QVector<double> X, QVector<double> Y, double Xval);

    UserSpectrumModel* model;
    QString spectrumfilepath;
    QTableView* table;
    QGridLayout* layout;
    QLineEdit *inputScript;
    QStringList varNamesAndValues;
    QRadioButton *option1Button, *option2Button, *option3Button;
    QLabel *labelProgName, *labelProgDir1, *labelProgDir2, *labelThresMsg , *qoiNames;
    bool isData;
    QLineEdit *thresVal;
    QGroupBox *groupBox;
    QVector<double> percVals, thrsVals;
    QLineEdit *postprocessScript;
    QComboBox * gpOutputComboBox;
    QStringList qoiNames_list;

    // random variable box
    QVBoxLayout *vLayout;
    QVBoxLayout *rvLayout;
    QGroupBox *rvBox;
    QList<QString> rvNames;
    QVector<double> rvValues;
    QList<QLineEdit *> rvValueLE;

    // periods
    QLineEdit *periodsLineEdit;
    QRegExpValidator *LEValidator;
    QList<double> periodArray;

    // run
    QProcess *proc;

};

#endif // SPECTRUMFROMREGIONALSURROGATE_H
