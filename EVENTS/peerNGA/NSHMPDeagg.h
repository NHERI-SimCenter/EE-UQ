#ifndef NSHMPTDeagg_H
#define NSHMPTDeagg_H
#include "AbstractTargetWidget.h"
#include <GeneralInformationWidget.h>
#include <QLineEdit>
#include <QComboBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QLabel>
#include <QJsonObject>
#include <QPushButton>

class NSHMPDeagg : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit NSHMPDeagg(GeneralInformationWidget* generalInfoWidget, QWidget* parent = nullptr);

private:
    QLineEdit* latitudeBox;
    QLineEdit* longitudeBox;
    QComboBox* vs30Box;
    QComboBox* editionBox;
    QComboBox* imtBox;
    QComboBox* gmpeBox;
    GeneralInformationWidget* generalInfoWidget;
    QLineEdit* returnPeriodBox;
    QLineEdit* TxBox;
    QLabel* TxLabel1;
    QLabel* TxLabel2;
    QList<double> availPeriod = {0.001, 0.1, 0.2, 0.3, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0};
    QDoubleValidator* TxValidator;

    QPushButton* seismicDisaggButton;

    mutable QNetworkAccessManager networkManager;

    QList<QPair<double, double>> getSpectrum(QJsonObject& json, double Tc, double Sac) const;
    QList<QPair<double, QList<double>>> getDeagg(QJsonObject& json) const;
    void SetComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled);

    // AbstractJsonSerializable interface
public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;

    // AbstractTargetWidget interface
public:
    QList<QPair<double, double>> spectrum() const override;
    QList<QPair<double, QList<double>>> deagginfo() const;
};

#endif // NSHMPTDeagg_H
