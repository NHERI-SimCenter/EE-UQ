#ifndef USGSTARGETWIDGET_H
#define USGSTARGETWIDGET_H
#include <QObject>

#include "AbstractTargetWidget.h"
#include <QLineEdit>
#include <QComboBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QLabel>
#include <GeneralInformationWidget.h>


class USGSTargetWidget : public AbstractTargetWidget
{
    Q_OBJECT

public:
    explicit USGSTargetWidget(GeneralInformationWidget* generalInfoWidget, QWidget* parent = nullptr);

    // AbstractJsonSerializable interface
public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;

    // AbstractTargetWidget interface
public:
    QList<QPair<double, double> > spectrum() const override;

private:
    QLineEdit* latitudeBox;
    QLineEdit* longitudeBox;
    QComboBox* designStandardBox;
    QComboBox* siteClassBox;
    QComboBox* riskCategoryBox;
    QLabel* riskCategoryLabel;
    QComboBox* performanceObjectiveBox;
    QLabel* performanceObjectiveLabel;
    GeneralInformationWidget* generalInfoWidget;

    mutable QNetworkAccessManager networkManager;
};

#endif // USGSTARGETWIDGET_H
