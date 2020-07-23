#ifndef NSHMPTARGET_H
#define NSHMPTARGET_H
#include "AbstractTargetWidget.h"
#include <GeneralInformationWidget.h>
#include <QLineEdit>
#include <QComboBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QLabel>
#include <QJsonObject>

class NSHMPTarget : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit NSHMPTarget(GeneralInformationWidget* generalInfoWidget, QWidget* parent = nullptr);

private:
    QLineEdit* latitudeBox;
    QLineEdit* longitudeBox;
    QComboBox* vs30Box;
    QComboBox* editionBox;
    QComboBox* returnPeriod;
    GeneralInformationWidget* generalInfoWidget;

    mutable QNetworkAccessManager networkManager;

    QList<QPair<double, double>> getUHS(QJsonObject& json) const;

    // AbstractJsonSerializable interface
public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;

    // AbstractTargetWidget interface
public:
    QList<QPair<double, double>> spectrum() const override;
};

#endif // NSHMPTARGET_H
