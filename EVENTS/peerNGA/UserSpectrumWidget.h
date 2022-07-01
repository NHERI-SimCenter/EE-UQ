#ifndef USERSPECTRUMWIDGET_H
#define USERSPECTRUMWIDGET_H

#include "AbstractTargetWidget.h"
#include "UserSpectrumModel.h"
#include <QFile>
#include <QDir>
#include <QTableView>
#include <QGridLayout>

class UserSpectrumWidget : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit UserSpectrumWidget(QWidget *parent = nullptr);

    // AbstractJsonSerializable interface
public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;

public slots:
    void loadCSV(void);
    void loadSpectrum(QString spectrumfilepath);

    // AbstractTargetWidget interface
public:
    QList<QPair<double, double> > spectrum() const override;

private:
    UserSpectrumModel* model;
    QString spectrumfilepath;
    QTableView* table;
    QGridLayout* layout;

};

#endif // USERSPECTRUMWIDGET_H
