#ifndef USERSPECTRUMWIDGET_H
#define USERSPECTRUMWIDGET_H

#include "AbstractTargetWidget.h"
#include "UserSpectrumModel.h"

class UserSpectrumWidget : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit UserSpectrumWidget(QWidget *parent = nullptr);

    // AbstractJsonSerializable interface
public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;

    // AbstractTargetWidget interface
public:
    QList<QPair<double, double> > spectrum() const override;

private:
    UserSpectrumModel* model;

};

#endif // USERSPECTRUMWIDGET_H
