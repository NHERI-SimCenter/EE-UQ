#include "NSHMPTarget.h"
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QtNetwork/QNetworkReply>
#include <GoogleAnalytics.h>

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
    editionBox->addItem("2014 v4.2.0 (Dynamic)", "https://earthquake.usgs.gov/nshmp-haz-ws/hazard/E2014B/COUS");
    editionBox->addItem("2014 v4.1.4 (Dynamic)", "https://earthquake.usgs.gov/nshmp-haz-ws/hazard/E2014/COUS");
    editionBox->addItem("2014 v4.0.0 (Static)", "https://earthquake.usgs.gov/hazws/staticcurve/1/E2014R1/COUS0P05");
    editionBox->addItem("2008 v3.3.3 (Dynamic)", "https://earthquake.usgs.gov/nshmp-haz-ws/hazard/E2008/COUS");
    editionBox->addItem("2008 v3.2.0 (Static)", "https://earthquake.usgs.gov/hazws/staticcurve/1/E2008R3/COUS0P05");
    editionBox->addItem("2008 v3.1.0 (Static)", "https://earthquake.usgs.gov/hazws/staticcurve/1/E2008R2/COUS0P05");


    layout->addWidget(new QLabel(tr("Vs30")), 3, 0);
    vs30Box = new QComboBox();
    layout->addWidget(vs30Box, 3, 1);
    vs30Box->addItem("180 (Stiff/Soft Soil D/E)", "180");
    vs30Box->addItem("259 (Stiff Soil D)", "259");
    vs30Box->addItem("360 (C/D)", "360");
    vs30Box->addItem("537 (Very Dense Soil/Soft Rock - C)", "537");
    vs30Box->addItem("760 (B/C)", "760");
    vs30Box->addItem("1150 (Rock - B)", "1150");
    vs30Box->addItem("2000 (Hard Rock - A)", "2000");
    layout->addWidget(new QLabel(tr("m/s")), 3, 2);
    vs30Box->setCurrentIndex(4);

    layout->addWidget(new QLabel(tr("Return Period")), 4, 0);
    returnPeriod = new QComboBox();
    layout->addWidget(returnPeriod, 4, 1);
    returnPeriod->addItem("500");
    returnPeriod->setItemData(0, "10% exceedance in 50 years", Qt::ToolTipRole);
    returnPeriod->addItem("1000");
    returnPeriod->setItemData(1, "5% exceedance in 50 years", Qt::ToolTipRole);
    returnPeriod->addItem("2500");
    returnPeriod->setItemData(2, "2% exceedance in 50 years", Qt::ToolTipRole);
    returnPeriod->setCurrentIndex(2);
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

    connect(editionBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        if(editionBox->itemData(index).toString().startsWith("https://earthquake.usgs.gov/hazws/staticcurve"))
        {
            vs30Box->setEnabled(false);
            vs30Box->setCurrentIndex(4);
        }
        else
            vs30Box->setEnabled(true);
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

        QJsonArray x, y;
        if(hazardCurve.toObject()["data"].toArray()[0].toObject().keys().contains("yvalues"))
            y = hazardCurve.toObject()["data"].toArray()[0].toObject()["yvalues"].toArray();
        else
            y = hazardCurve.toObject()["data"].toArray()[0].toObject()["yvals"].toArray();

        if(hazardCurve.toObject()["metadata"].toObject().keys().contains("xvalues"))
            x = hazardCurve.toObject()["metadata"].toObject()["xvalues"].toArray();
        else
            x = hazardCurve.toObject()["metadata"].toObject()["xvals"].toArray();

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

    if(spectrum.empty())
        const_cast<NSHMPTarget*>(this)->emit statusUpdated("USGS NSHMP Error: Failed to retrieve uniform hazard spectrum with the inputs provided");

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
    GoogleAnalytics::Report("RecordSelection", "USGS-UHS");

    QString url(editionBox->currentData().toString());

    url.append("/" + longitudeBox->text());
    url.append("/" + latitudeBox->text());
    url.append("/any");
    url.append("/" + vs30Box->currentData().toString());

    QNetworkRequest request(url);

    auto reply = networkManager.get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject replyJson = QJsonDocument::fromJson(reply->readAll()).object();

    if(reply->error() != QNetworkReply::NoError)
    {
        return QList<QPair<double, double>>();
    }
    else if(!replyJson.keys().contains("response"))
    {
        if (replyJson.keys().contains("status") && replyJson["status"].toString() == "busy")
            const_cast<NSHMPTarget*>(this)->emit statusUpdated("USGS NSHMP Error: " + replyJson["message"].toString());
        return QList<QPair<double, double>>();
    }

    return getUHS(replyJson);
}
