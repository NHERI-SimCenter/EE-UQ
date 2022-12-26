#ifndef NoSpectrumUniform_H
#define NoSpectrumUniform_H

#include <QWidget>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include "AbstractTargetWidget.h"
#include "SimCenterIntensityMeasureWidget.h"

class NoSpectrumUniform : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit NoSpectrumUniform(QWidget *parent = nullptr);
    void writeConfigJSON(QJsonObject myJson);
    void getUniformRSN(void);
signals:
    void runComplete(bool, QString, QString);

private:
//    QLineEdit* sdsEditBox;
//    QLineEdit* sd1EditBox;
//    QLineEdit* tlEditBox;

    QLineEdit* numSampPerBin;
    SimCenterIntensityMeasureWidget *theSCIMWidget_grid;
    QPushButton* selectRecordsButton;
    QProcess * proc;

public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;


    // AbstractTargetWidget interface
public:
    QList<QPair<double, double> > spectrum() const override;
    QStringList getRSN() const;
//    double sds();
//    double sd1();
//    double tl();

};

#endif // NoSpectrumUniform_H
