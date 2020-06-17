#include "USGSTargetWidget.h"
#include <QJsonObject>
#include <QGridLayout>
#include <QLabel>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

USGSTargetWidget::USGSTargetWidget(QWidget* parent):AbstractTargetWidget(parent)
{
    auto layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Latitude")), 0, 0);
    latitudeBox = new QLineEdit("37.8719");
    layout->addWidget(latitudeBox, 0, 1);

    layout->addWidget(new QLabel(tr("Longitude")), 1, 0);
    longitudeBox = new QLineEdit("-122.2585");
    layout->addWidget(longitudeBox, 1, 1);

    layout->addWidget(new QLabel(tr("Design Standard")), 2, 0);
    designStandardBox = new QComboBox();
    layout->addWidget(designStandardBox, 2, 1);
    designStandardBox->addItem("ASCE7-16");
    designStandardBox->addItem("ASCE7-10");
    designStandardBox->addItem("ASCE7-05");
    designStandardBox->addItem("IBC-2015");
    designStandardBox->addItem("IBC-2012");
    designStandardBox->addItem("NEHRP-2020");
    designStandardBox->addItem("NEHRP-2015");
    designStandardBox->addItem("NEHRP-2009");

    layout->addWidget(new QLabel(tr("Site Class")), 3, 0);
    siteClassBox = new QComboBox();
    layout->addWidget(siteClassBox, 3, 1);
    siteClassBox->addItem("A");
    siteClassBox->addItem("B");
    siteClassBox->addItem("C");
    siteClassBox->addItem("D");
    siteClassBox->addItem("E");

    layout->addWidget(new QLabel(tr("Risk Category")), 4, 0);
    riskCategoryBox = new QComboBox();
    layout->addWidget(riskCategoryBox, 4, 1);
    riskCategoryBox->addItem("I");
    riskCategoryBox->addItem("II");
    riskCategoryBox->addItem("III");
    riskCategoryBox->addItem("IV");

    layout->setRowStretch(layout->rowCount(), 1);

}


QJsonObject USGSTargetWidget::serialize() const
{
    QJsonObject json;
    return json;
}

void USGSTargetWidget::deserialize(const QJsonObject &json)
{

}

QList<QPair<double, double> > USGSTargetWidget::spectrum() const
{
    QList<QPair<double, double>> spectrum;
    QUrl usgswsUrl("https://earthquake.usgs.gov/ws/designmaps/" + designStandardBox->currentText().toLower() +".json");
    QUrlQuery parameters;
    parameters.addQueryItem("latitude", latitudeBox->text());
    parameters.addQueryItem("longitude", longitudeBox->text());
    parameters.addQueryItem("siteClass", siteClassBox->currentText());
    parameters.addQueryItem("riskCategory", riskCategoryBox->currentText());
    parameters.addQueryItem("title", "EEUQ");

    usgswsUrl.setQuery(parameters);
    QNetworkRequest request(usgswsUrl);

    auto reply = networkManager.get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject replyJson = QJsonDocument::fromJson(reply->readAll()).object();

    //auto designSpectrum = replyJson["response"].toObject()["data"].toObject()["twoPeriodMCErSpectrum"].toArray();
    auto designSpectrum = replyJson["response"].toObject()["data"].toObject()["twoPeriodDesignSpectrum"].toArray();

    for (auto point: designSpectrum)
        spectrum.append({point.toArray()[0].toDouble(), point.toArray()[1].toDouble()});

    return spectrum;
}
