#include "NSHMPTarget.h"
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QtNetwork/QNetworkReply>
#include <GoogleAnalytics.h>
#include <math.h>
#include "UsgsNshmpService.h"

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
    // USGS retired the 2014/2008 nshmp-haz-ws and hazws services (Nov 2026); these are the
    // models offered by the current https://earthquake.usgs.gov/ws/nshmp services
    editionBox->addItem("Conterminous U.S. 2023", UsgsNshmp::baseUrl + "/conus-2023/dynamic");
    editionBox->addItem("Conterminous U.S. 2018", UsgsNshmp::baseUrl + "/conus-2018/dynamic");
    editionBox->addItem("Alaska 2023", UsgsNshmp::baseUrl + "/alaska-2023/dynamic");
    editionBox->addItem("Hawaii 2021", UsgsNshmp::baseUrl + "/hawaii-2021/dynamic");


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

    /*
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
    */

    auto positiveIntegerValidator = new QIntValidator(1, 9999); // USGS limits the return period from 2 to 9999
    positiveIntegerValidator->setBottom(2);
    layout->addWidget(new QLabel(tr("Return Period")), 4, 0);
    returnPeriodBox = new QLineEdit("2475"); // default 2475 year
    returnPeriodBox->setValidator(positiveIntegerValidator);
    layout->addWidget(returnPeriodBox, 4, 1);
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
    // { "response": { "hazardCurves": [ { "imt": {"value": "SA1P0"},
    //                                     "data": [ {"component": "Total", "values": {"xs": [...], "ys": [...]}}, ... ] } ] } }
    auto hazardCurves = json["response"].toObject()["hazardCurves"].toArray();

    //double exceedanceProb = 1.0/returnPeriod->currentText().toDouble();
    // Add an additional check of the return period range accepted by USGS
    if (returnPeriodBox->text().isEmpty() || returnPeriodBox->text().toInt() < 2)
    {
        const_cast<NSHMPTarget*>(this)->emit statusUpdated("Please input a valid return period (2~9999).");
        return spectrum;
    }
    double exceedanceProb = 1.0/returnPeriodBox->text().toDouble();
    for (auto hazardCurve: hazardCurves)
    {
        auto imt = hazardCurve.toObject()["imt"].toObject()["value"].toString();

        // skip PGA and PGV: the service now also returns SA0P01, which gives the 0.01s point
        // (previously PGA was used as 0.01s, so it would now duplicate that period)
        double period = UsgsNshmp::imtPeriod(imt);
        if (period <= 0)
            continue;

        // total hazard curve (other components are per source type)
        QJsonArray x, y;
        for (auto component : hazardCurve.toObject()["data"].toArray()) {
            if (component.toObject()["component"].toString() == "Total") {
                x = component.toObject()["values"].toObject()["xs"].toArray();
                y = component.toObject()["values"].toObject()["ys"].toArray();
                break;
            }
        }

        for(int i = y.size()-1; i > 0; i--)
        {
            if(exceedanceProb >= y[i].toDouble() && exceedanceProb <= y[i-1].toDouble())
            {
                // interpolation in log-log space
                double delta = (log(exceedanceProb) - log(y[i].toDouble())) /(log(y[i-1].toDouble()) - log(y[i].toDouble()));
                double im = exp(log(x[i].toDouble()) + delta * (log(x[i-1].toDouble())-log(x[i].toDouble())));
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
    //json["ReturnPeriod"] = returnPeriod->currentText();
    json["ReturnPeriod"] = returnPeriodBox->text();

    return json;
}

void NSHMPTarget::deserialize(const QJsonObject &json)
{
    vs30Box->setCurrentText(json["Vs30"].toString());
    editionBox->setCurrentText(json["Edition"].toString());
    //returnPeriod->setCurrentText(json["ReturnPeriod"].toString());
    returnPeriodBox->setText(json["ReturnPeriod"].toString());

    double latitude, longitude;
    generalInfoWidget->getBuildingLocation(latitude, longitude);
    latitudeBox->setText(QString::number(latitude));
    longitudeBox->setText(QString::number(longitude));
}

QList<QPair<double, double>> NSHMPTarget::spectrum() const
{
    GoogleAnalytics::Report("RecordSelection", "USGS_UHS");

    // {model}/dynamic/hazard/{longitude}/{latitude}/{vs30}  (all IMTs returned)
    QString url(editionBox->currentData().toString());
    url.append("/hazard");
    url.append("/" + longitudeBox->text());
    url.append("/" + latitudeBox->text());
    url.append("/" + vs30Box->currentData().toString());

    qDebug() << "USGS hazard URL = " + url;

    QString errorMessage;
    QJsonObject replyJson = UsgsNshmp::getJson(networkManager, QUrl(url), errorMessage);

    if (!errorMessage.isEmpty() || !replyJson["response"].isObject())
    {
        const_cast<NSHMPTarget*>(this)->emit statusUpdated("USGS NSHMP Error: " +
            (errorMessage.isEmpty() ? QString("unexpected reply from hazard service") : errorMessage));
        return QList<QPair<double, double>>();
    }

    return getUHS(replyJson);
}
