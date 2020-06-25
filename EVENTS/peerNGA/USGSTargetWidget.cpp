#include "USGSTargetWidget.h"
#include <QJsonObject>
#include <QGridLayout>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <GoogleAnalytics.h>

USGSTargetWidget::USGSTargetWidget(GeneralInformationWidget* generalInfoWidget, QWidget* parent):AbstractTargetWidget(parent), generalInfoWidget(generalInfoWidget)
{
    auto layout = new QGridLayout(this);
    layout->setColumnMinimumWidth(2, 30);

    double latitude, longitude;
    generalInfoWidget->getBuildingLocation(latitude, longitude);
    layout->addWidget(new QLabel(tr("Latitude")), 0, 0);
    latitudeBox = new QLineEdit("37.8719");
    layout->addWidget(latitudeBox, 0, 1);
    latitudeBox->setText(QString::number(latitude));

    layout->addWidget(new QLabel(tr("Longitude")), 1, 0);
    longitudeBox = new QLineEdit("-122.2585");
    layout->addWidget(longitudeBox, 1, 1);
    longitudeBox->setText(QString::number(longitude));

    layout->addWidget(new QLabel(tr("Standard")), 2, 0);
    designStandardBox = new QComboBox();
    layout->addWidget(designStandardBox, 2, 1);
    designStandardBox->addItem("ASCE 7-16", "asce7-16");
    designStandardBox->addItem("ASCE 7-10", "asce7-10");
    designStandardBox->addItem("ASCE 7-05", "asce7-05");
    designStandardBox->addItem("ASCE 41-17", "asce41-17");
    designStandardBox->addItem("ASCE 41-13", "asce41-13");
    designStandardBox->addItem("IBC 2015", "ibc-2015");
    designStandardBox->addItem("IBC 2012", "ibc-2012");
    designStandardBox->addItem("NEHRP 2020", "nehrp-2020");
    designStandardBox->addItem("NEHRP 2015", "nehrp-2015");
    designStandardBox->addItem("NEHRP 2009", "nehrp-2009");

    layout->addWidget(new QLabel(tr("Site Class")), 3, 0);
    siteClassBox = new QComboBox();
    layout->addWidget(siteClassBox, 3, 1);
    siteClassBox->addItem("Hard Rock (A)", "A");
    siteClassBox->addItem("Rock (B)", "B");
    siteClassBox->addItem("Very Dense Soil/Soft Rock (C)", "C");
    siteClassBox->addItem("Stiff Soil (D)", "D");
    siteClassBox->addItem("Soft Soil (E)", "E");

    riskCategoryLabel = new QLabel(tr("Risk Category"));
    layout->addWidget(riskCategoryLabel, 4, 0);
    riskCategoryBox = new QComboBox();
    layout->addWidget(riskCategoryBox, 4, 1);
    riskCategoryBox->addItem("I (Low Hazard)", "I");
    riskCategoryBox->addItem("II", "II");
    riskCategoryBox->addItem("III (Substantial Hazard)", "III");
    riskCategoryBox->addItem("IV (Essential Facilities)", "IV");

    performanceObjectiveLabel = new QLabel(tr("Performance"));
    layout->addWidget(performanceObjectiveLabel, 5, 0);
    performanceObjectiveBox = new QComboBox();
    layout->addWidget(performanceObjectiveBox, 5, 1);
    performanceObjectiveBox->addItem("BSE-1E");
    performanceObjectiveBox->addItem("BSE-2E");
    performanceObjectiveBox->addItem("BSE-1N");
    performanceObjectiveBox->addItem("BSE-2N");
    performanceObjectiveBox->setVisible(false);
    performanceObjectiveLabel->setVisible(false);

    layout->setRowStretch(layout->rowCount(), 1);

    connect(designStandardBox, &QComboBox::currentTextChanged, this, [this](const QString standard){

        Q_UNUSED(standard)
        if (designStandardBox->currentData().toString().startsWith("asce41"))
        {
            performanceObjectiveBox->setVisible(true);
            performanceObjectiveLabel->setVisible(true);
            riskCategoryBox->setVisible(false);
            riskCategoryLabel->setVisible(false);
        }
        else
        {
            performanceObjectiveBox->setVisible(false);
            performanceObjectiveLabel->setVisible(false);
            riskCategoryBox->setVisible(true);
            riskCategoryLabel->setVisible(true);
        }

    });

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


QJsonObject USGSTargetWidget::serialize() const
{
    QJsonObject json;

    json["DesignStandard"] = designStandardBox->currentText();
    json["SiteClass"] = siteClassBox->currentText();
    if (designStandardBox->currentData().toString().startsWith("asce41"))
        json["PerformanceObjective"] = performanceObjectiveBox->currentText();
    else
        json["RiskCategory"] = riskCategoryBox->currentText();

    return json;
}

void USGSTargetWidget::deserialize(const QJsonObject &json)
{
    designStandardBox->setCurrentText(json["DesignStandard"].toString());
    siteClassBox->setCurrentText(json["SiteClass"].toString());

    if (designStandardBox->currentData().toString().startsWith("asce41"))
        performanceObjectiveBox->setCurrentText(json["PerformanceObjective"].toString());
    else
        riskCategoryBox->setCurrentText(json["RiskCategory"].toString());

    double latitude, longitude;
    generalInfoWidget->getBuildingLocation(latitude, longitude);
    latitudeBox->setText(QString::number(latitude));
    longitudeBox->setText(QString::number(longitude));
}

QList<QPair<double, double>> USGSTargetWidget::spectrum() const
{
    GoogleAnalytics::Report("RecordSelection", "USGS-DesignMaps");
    QList<QPair<double, double>> spectrum;
    QUrl usgswsUrl("https://earthquake.usgs.gov/ws/designmaps/" + designStandardBox->currentData().toString() +".json");
    QUrlQuery parameters;
    parameters.addQueryItem("latitude", latitudeBox->text());
    parameters.addQueryItem("longitude", longitudeBox->text());
    parameters.addQueryItem("siteClass", siteClassBox->currentData().toString());
    if(!designStandardBox->currentData().toString().startsWith("asce41"))
        parameters.addQueryItem("riskCategory", riskCategoryBox->currentData().toString());
    parameters.addQueryItem("title", "EEUQ");

    usgswsUrl.setQuery(parameters);
    QNetworkRequest request(usgswsUrl);

    auto reply = networkManager.get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() != QNetworkReply::NoError)
    {
        const_cast<USGSTargetWidget*>(this)->emit statusUpdated("USGS DesignMaps Error: Failed to retrieve design spectrum with the inputs provided");
        return spectrum;
    }

    QJsonObject replyJson = QJsonDocument::fromJson(reply->readAll()).object();

    //auto designSpectrum = replyJson["response"].toObject()["data"].toObject()["twoPeriodMCErSpectrum"].toArray();

    QJsonArray designSpectrum;
    if(designStandardBox->currentData().toString().startsWith("asce41"))
    {
        for (auto hazardLevelObjective: replyJson["response"].toObject()["data"].toArray())
            if(hazardLevelObjective.toObject()["hazardLevel"].toString().compare(performanceObjectiveBox->currentText()) == 0)
                designSpectrum = hazardLevelObjective.toObject()["horizontalSpectrum"].toArray();
    }
    else
        designSpectrum = replyJson["response"].toObject()["data"].toObject()["twoPeriodDesignSpectrum"].toArray();

    for (auto point: designSpectrum)
        spectrum.append({point.toArray()[0].toDouble(), point.toArray()[1].toDouble()});

    if(spectrum.empty())
        const_cast<USGSTargetWidget*>(this)->emit statusUpdated("USGS DesignMaps Error: Failed to retrieve design spectrum with the inputs provided");


    return spectrum;
}
