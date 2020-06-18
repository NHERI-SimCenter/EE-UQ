#include "NSHMPTarget.h"
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QtNetwork/QNetworkReply>

NSHMPTarget::NSHMPTarget(GeneralInformationWidget* generalInfoWidget, QWidget* parent):AbstractTargetWidget(parent), generalInfoWidget(generalInfoWidget)
{
    auto layout = new QGridLayout(this);
    layout->setColumnMinimumWidth(2, 30);

    double latitude, longitude;
    generalInfoWidget->getBuildingLocation(latitude, longitude);
    layout->addWidget(new QLabel(tr("Latitude")), 0, 0);
    latitudeBox = new QLineEdit();
    layout->addWidget(latitudeBox, 0, 1);
    latitudeBox->setText(QString::number(latitude));

    layout->addWidget(new QLabel(tr("Longitude")), 1, 0);
    longitudeBox = new QLineEdit();
    layout->addWidget(longitudeBox, 1, 1);
    longitudeBox->setText(QString::number(longitude));

    layout->addWidget(new QLabel(tr("Edition")), 2, 0);
    editionBox = new QComboBox();
    layout->addWidget(editionBox, 2, 1);
    editionBox->addItem("2014");
    editionBox->addItem("2008");


    layout->addWidget(new QLabel(tr("Vs30")), 3, 0);
    vs30Box = new QComboBox();
    layout->addWidget(vs30Box, 3, 1);
    vs30Box->addItem("180");
    vs30Box->addItem("259");
    vs30Box->addItem("360");
    vs30Box->addItem("537");
    vs30Box->addItem("760");
    vs30Box->addItem("1150");
    vs30Box->addItem("2000");

    layout->addWidget(new QLabel(tr("Return Period")), 4, 0);
    returnPeriod = new QComboBox();
    layout->addWidget(returnPeriod, 4, 1);
    returnPeriod->addItem("500");
    returnPeriod->addItem("1000");
    returnPeriod->addItem("2500");
    layout->addWidget(new QLabel(tr("years")), 4, 2);


    layout->setRowStretch(layout->rowCount(), 1);


    connect(generalInfoWidget, &GeneralInformationWidget::buildingLocationChanged, this, [this](double latitude, double longitude){
        latitudeBox->setText(QString::number(latitude));
        longitudeBox->setText(QString::number(longitude));
    });

    connect(latitudeBox, &QLineEdit::editingFinished, this, [this, generalInfoWidget](){
        generalInfoWidget->setBuildingLocation(latitudeBox->text().toDouble(), longitudeBox->text().toDouble());
    });

    connect(longitudeBox, &QLineEdit::editingFinished, this, [this, generalInfoWidget](){
        generalInfoWidget->setBuildingLocation(latitudeBox->text().toDouble(), longitudeBox->text().toDouble());
    });
}

QList<QPair<double, double>> NSHMPTarget::getUHS(QJsonObject& json) const
{
    QList<QPair<double, double>> spectrum;
    auto hazardCurves = json["response"].toArray();

    double exceedanceProb = 1.0/returnPeriod->currentText().toDouble();
    for (auto hazardCurve: hazardCurves)
    {
        auto imt = hazardCurve.toObject()["metadata"].toObject()["imt"].toObject()["value"].toString();
        double period = 0;
        if (imt.startsWith("SA"))
            period = imt.remove("SA").replace('P', '.').toDouble();

        auto y = hazardCurve.toObject()["data"].toArray()[0].toObject()["yvalues"].toArray();
        auto x = hazardCurve.toObject()["metadata"].toObject()["xvalues"].toArray();

        for(int i = y.size()-1; i > 0; i--)
        {
            if(exceedanceProb >= y[i].toDouble() && exceedanceProb <= y[i-1].toDouble())
            {
                double delta = (exceedanceProb - y[i].toDouble()) /(y[i-1].toDouble() - y[i].toDouble());
                double im = x[i].toDouble() + delta * (x[i-1].toDouble()-x[i].toDouble());
                spectrum.append({period, im});
                break;
            }
        }
    }


    return spectrum;
}


QJsonObject NSHMPTarget::serialize() const
{
    QJsonObject json;

    json["Vs30"] = vs30Box->currentText();
    json["Edition"] = editionBox->currentText();
    json["ReturnPeriod"] = returnPeriod->currentText();

    return json;
}

void NSHMPTarget::deserialize(const QJsonObject &json)
{
    vs30Box->setCurrentText(json["Vs30"].toString());
    editionBox->setCurrentText(json["Edition"].toString());
    returnPeriod->setCurrentText(json["ReturnPeriod"].toString());

    double latitude, longitude;
    generalInfoWidget->getBuildingLocation(latitude, longitude);
    latitudeBox->setText(QString::number(latitude));
    longitudeBox->setText(QString::number(longitude));
}

QList<QPair<double, double>> NSHMPTarget::spectrum() const
{

    QString url("https://earthquake.usgs.gov/nshmp-haz-ws/hazard");
    url.append("/E" + editionBox->currentText());
    if(editionBox->currentText().compare("2014") == 0)
    url.append('B');

    double longitude = longitudeBox->text().toDouble();
    if (longitude <= -100.0 && longitude >= -125.0)
        url.append("/WUS");
    else if (longitude <= -65.0 && longitude >= -115.0)
        url.append("/CEUS");
    else
        url.append("/COUS");

    url.append("/" + longitudeBox->text());
    url.append("/" + latitudeBox->text());
    url.append("/ANY");
    url.append("/" + vs30Box->currentText());

    QNetworkRequest request(url);

    auto reply = networkManager.get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject replyJson = QJsonDocument::fromJson(reply->readAll()).object();



    return getUHS(replyJson);
}
