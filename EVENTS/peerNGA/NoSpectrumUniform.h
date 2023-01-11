#ifndef NoSpectrumUniform_H
#define NoSpectrumUniform_H

#include <QWidget>
#include <QLineEdit>
#include <QProcess>
#include <QLabel>
#include <QPushButton>
#include "AbstractTargetWidget.h"
#include "SimCenterIntensityMeasureWidget.h"

class NoSpectrumUniform : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit NoSpectrumUniform(QWidget *parent = nullptr);
    bool writeConfigJSON(QJsonObject &myJson);

signals:
    void runComplete(bool, QString, QString);

public slots:
    void updateNumTotalSamp(int);

private:

    QLineEdit* numSampPerBin;
    QLabel* numTotalSamp;
    SimCenterIntensityMeasureWidget *theSCIMWidget_grid;
    QPushButton* selectRecordsButton;
    QProcess * proc;

public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;


    // AbstractTargetWidget interface
    QList<QPair<double, double> > spectrum() const override;
    void getRSN(QString WorkDir, QStringList &RSN, QVector<double> &additionalScaling, QString &imagePath);
//    double sds();
//    double sd1();
//    double tl();

};

#endif // NoSpectrumUniform_H
