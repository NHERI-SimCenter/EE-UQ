#include "ASCE710Target.h"
#include <QGridLayout>
#include <QLabel>
#include <QDoubleValidator>

ASCE710Target::ASCE710Target(QWidget *parent) : AbstractTargetWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto positiveDoubleValidator = new QDoubleValidator();
    positiveDoubleValidator->setBottom(0.0);

    layout->addWidget(new QLabel("S<sub>DS</sub>"), 0, 0);
    sdsEditBox = new QLineEdit("1.0");
    sdsEditBox->setValidator(positiveDoubleValidator);
    layout->addWidget(sdsEditBox, 0, 1);
    layout->addWidget(new QLabel("g"), 0, 2);

    layout->addWidget(new QLabel("S<sub>D1</sub>"), 1, 0);
    sd1EditBox = new QLineEdit("0.75");
    sd1EditBox->setValidator(positiveDoubleValidator);
    layout->addWidget(sd1EditBox, 1, 1);
    layout->addWidget(new QLabel("g"), 1, 2);

    layout->addWidget(new QLabel("T<sub>L</sub>"), 2, 0);
    tlEditBox = new QLineEdit("12.0");
    tlEditBox->setValidator(positiveDoubleValidator);
    layout->addWidget(tlEditBox, 2, 1);
    layout->addWidget(new QLabel("sec."), 2, 2);
    layout->setRowStretch(layout->rowCount(), 1);
}


QJsonObject ASCE710Target::serialize() const
{
    QJsonObject json;
    json["Sds"] = sdsEditBox->text();
    json["Sd1"] = sd1EditBox->text();
    json["Tl"] = tlEditBox->text();
    return json;
}

void ASCE710Target::deserialize(const QJsonObject &json)
{
    sdsEditBox->setText(json["Sds"].toString());
    sd1EditBox->setText(json["Sd1"].toString());
    tlEditBox->setText(json["Tl"].toString());
}

QList<QPair<double, double>> ASCE710Target::spectrum() const
{
    QList<QPair<double, double>>  targetSectrum;
    return targetSectrum;
}

double ASCE710Target::sds()
{
    return sdsEditBox->text().toDouble();
}

double ASCE710Target::sd1()
{
    return sd1EditBox->text().toDouble();
}

double ASCE710Target::tl()
{
    return tlEditBox->text().toDouble();
}
