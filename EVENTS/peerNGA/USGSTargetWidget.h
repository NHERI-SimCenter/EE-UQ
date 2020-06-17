#ifndef USGSTARGETWIDGET_H
#define USGSTARGETWIDGET_H
#include <QObject>

#include "AbstractTargetWidget.h"
#include <QLineEdit>
#include <QComboBox>
#include <QtNetwork/QNetworkAccessManager>


class USGSTargetWidget : public AbstractTargetWidget
{
    Q_OBJECT

public:
    explicit USGSTargetWidget(QWidget* parent = nullptr);

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

    mutable QNetworkAccessManager networkManager;
};

#endif // USGSTARGETWIDGET_H
