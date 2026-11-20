#include "NSHMPDeagg.h"
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QtNetwork/QNetworkReply>
#include <GoogleAnalytics.h>
#include <math.h>
#include <QStandardItemModel>
#include "groundMotionModel/BakerJayaram2008.h"
#include "UsgsNshmpService.h"
#include <algorithm>
#include <cassert>

NSHMPDeagg::NSHMPDeagg(GeneralInformationWidget* generalInfoWidget, QWidget* parent):AbstractTargetWidget(parent), generalInfoWidget(generalInfoWidget)
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

    layout->addWidget(new QLabel(tr("Vs30")), 2, 0);
    vs30Box = new QComboBox();
    layout->addWidget(vs30Box, 2, 1);
    vs30Box->addItem("180 (Stiff/Soft Soil D/E)", "180");
    vs30Box->addItem("259 (Stiff Soil D)", "259");
    vs30Box->addItem("360 (C/D)", "360");
    vs30Box->addItem("537 (Very Dense Soil/Soft Rock - C)", "537");
    vs30Box->addItem("760 (B/C)", "760");
    vs30Box->addItem("1150 (Rock - B)", "1150");
    vs30Box->addItem("2000 (Hard Rock - A)", "2000");
    layout->addWidget(new QLabel(tr("m/s")), 2, 2);
    vs30Box->setCurrentIndex(4);

    layout->addWidget(new QLabel(tr("Disagg. NSHM")), 3, 0);
    editionBox = new QComboBox();
    layout->addWidget(editionBox, 3, 1);
    // USGS retired the 2014/2008/2007 nshmp-haz-ws services (Nov 2026); these are the models
    // offered by the current https://earthquake.usgs.gov/ws/nshmp services. All of them
    // provide every period in imtBox below.
    editionBox->addItem("Conterminous U.S. 2023", UsgsNshmp::baseUrl + "/conus-2023/dynamic");
    editionBox->addItem("Conterminous U.S. 2018", UsgsNshmp::baseUrl + "/conus-2018/dynamic");
    editionBox->addItem("Alaska 2023", UsgsNshmp::baseUrl + "/alaska-2023/dynamic");
    editionBox->addItem("Hawaii 2021", UsgsNshmp::baseUrl + "/hawaii-2021/dynamic");
    editionBox->setCurrentIndex(0);

    auto positiveIntegerValidator = new QIntValidator(1, 9999); // USGS limits the return period from 2 to 9999
    positiveIntegerValidator->setBottom(2);
    layout->addWidget(new QLabel(tr("Return Period")), 4, 0);
    returnPeriodBox = new QLineEdit("2475"); // default 2475 year
    returnPeriodBox->setValidator(positiveIntegerValidator);
    layout->addWidget(returnPeriodBox, 4, 1);
    layout->addWidget(new QLabel(tr("years")), 4, 2);

    layout->addWidget(new QLabel(tr("GM Model")), 5, 0);
    gmpeBox = new QComboBox();
    layout->addWidget(gmpeBox, 5, 1);
    // the COMBINED_* and NGA_SUB_USGS_* models are not offered by the new gmm service
    const QString gmmUrl = UsgsNshmp::baseUrl + "/gmm/spectra?gmm=";
    gmpeBox->addItem("Boore, Stewart, Seyhan & Atkinson (2014)", gmmUrl + "BSSA_14");
    gmpeBox->addItem("Boore, Stewart, Seyhan & Atkinson (2014) : Basin", gmmUrl + "BSSA_14_BASIN");
    gmpeBox->addItem("Campbell & Bozorgnia (2014)", gmmUrl + "CB_14");
    gmpeBox->addItem("Campbell & Bozorgnia (2014) : Basin", gmmUrl + "CB_14_BASIN");
    gmpeBox->addItem("Chiou & Youngs (2014)", gmmUrl + "CY_14");
    gmpeBox->addItem("Chiou & Youngs (2014) : Basin", gmmUrl + "CY_14_BASIN");
    gmpeBox->addItem("Abrahamson, Silva & Kamai (2014)", gmmUrl + "ASK_14");
    gmpeBox->addItem("Abrahamson, Silva & Kamai (2014) : Basin", gmmUrl + "ASK_14_BASIN");
    gmpeBox->addItem("Idriss (2014)", gmmUrl + "IDRISS_14");
    gmpeBox->setCurrentIndex(0);

    layout->addWidget(new QLabel(tr("Conditional IM")), 6, 0);
    imtBox = new QComboBox();
    layout->addWidget(imtBox, 6, 1);
    imtBox->addItem("Peak Ground Acceleration", "PGA");
    imtBox->addItem("Sa(0.1s)", "SA0P1");
    imtBox->addItem("Sa(0.2s)", "SA0P2");
    imtBox->addItem("Sa(0.3s)", "SA0P3");
    imtBox->addItem("Sa(0.5s)", "SA0P5");
    imtBox->addItem("Sa(0.75s)", "SA0P75");
    imtBox->addItem("Sa(1.0s)", "SA1P0");
    imtBox->addItem("Sa(2.0s)", "SA2P0");
    imtBox->addItem("Sa(3.0s)", "SA3P0");
    imtBox->addItem("Sa(4.0s)", "SA4P0");
    imtBox->addItem("Sa(5.0s)", "SA5P0");
    imtBox->addItem("Sa(Tx)", "SATX");
    imtBox->setCurrentIndex(0);

    TxValidator = new QDoubleValidator(0.001, 5.0, 3);
    TxValidator->setNotation(QDoubleValidator::StandardNotation);
    TxLabel1 = new QLabel(tr("Tx"));
    layout->addWidget(TxLabel1, 7, 0);
    TxBox = new QLineEdit("1.0");
    TxBox->setValidator(TxValidator);
    layout->addWidget(TxBox, 7, 1);
    TxLabel2 = new QLabel(tr("sec"));
    layout->addWidget(TxLabel2, 7, 2);
    TxLabel1->setVisible(false);
    TxLabel2->setVisible(false);
    TxBox->setVisible(false);

    seismicDisaggButton = new QPushButton(tr("Disagg. Hazard"));
    layout->addWidget(seismicDisaggButton, 8, 0);

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

    // (the per-edition enabling/disabling of periods & Vs30 is gone: every current USGS
    //  model supports all the periods in imtBox, i.e. availPeriod = 0.001 (PGA) ... 5.0s)

    connect(imtBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        if(imtBox->itemData(index).toString().startsWith("SATX"))
        {
            TxLabel1->setVisible(true);
            TxLabel2->setVisible(true);
            TxBox->setVisible(true);
        } else {
            TxLabel1->setVisible(false);
            TxLabel2->setVisible(false);
            TxBox->setVisible(false);
        }
    });

    connect(seismicDisaggButton, &QPushButton::clicked, this, [this]() {
        this->deagginfo();
    });
}

QList<QPair<double, QList<double>>> NSHMPDeagg::getDeagg(QJsonObject& json) const
{
    qDebug() << "Processing USGS sesimic disaggregation results...";
    QList<QPair<double, QList<double>>> deagginfo_set;
    QList<QPair<double, QList<double>>> deagginfo;
    // { "response": { "metadata": {...}, "disaggs": [ { "imt": {"value": "SA1P0"},
    //     "data": [ { "component": "Total", "summary": [ ... ] } ] } ] } }
    // summary[0] = targets (data[2] = ground motion), summary[3] = mean (m, r, eps0)
    auto deaggRes = json["response"].toObject()["disaggs"].toArray();
    // Add an additional check of the return period range accepted by USGS
    if (returnPeriodBox->text().isEmpty() || returnPeriodBox->text().toInt() < 2)
    {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Please input a valid return period (2~9999).");
        return deagginfo;
    }
    if (TxBox->text().isEmpty() || TxBox->text().toDouble() > availPeriod.last())
    {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Please input a valid Tx (0.001~+"+ QString::number(availPeriod.last())+ ").");
        return deagginfo;
    }
    for (auto curDeagg: deaggRes)
    {
        auto imt = curDeagg.toObject()["imt"].toObject()["value"].toString();
        // PGA -> 0.01s (as NSHMPTarget does): getSpectrum() interpolates the GMM spectrum in
        // log(T) at this period, which starts at 0.01s, so 0 would never be bracketed
        double period = UsgsNshmp::imtPeriod(imt);
        if (period <= 0)
            period = 0.01;
        qDebug() << "Conditional period T = " + QString::number(period);
        // psa
        double psa = curDeagg.toObject()["data"].toArray()[0].toObject()["summary"].toArray()[0].toObject()["data"].toArray()[2].toObject()["value"].toDouble();
        // mean M, R, epsilon
        double meanM = curDeagg.toObject()["data"].toArray()[0].toObject()["summary"].toArray()[3].toObject()["data"].toArray()[0].toObject()["value"].toDouble();
        double meanR = curDeagg.toObject()["data"].toArray()[0].toObject()["summary"].toArray()[3].toObject()["data"].toArray()[1].toObject()["value"].toDouble();
        double meanE = curDeagg.toObject()["data"].toArray()[0].toObject()["summary"].toArray()[3].toObject()["data"].toArray()[2].toObject()["value"].toDouble();
        qDebug() << "Disaggregated Sa, M, R, E = " + QString::number(psa) + ", " + QString::number(meanM) + ", " + QString::number(meanR) + ", " + QString::number(meanE);
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("USGS Disaggregation: RP, T, Sa, M, R, epsilon = " + returnPeriodBox->text() + "yr, " + QString::number(period) +"s, " + QString::number(psa) + "g, Mw" + QString::number(meanM) + ", " + QString::number(meanR) + "km, " + QString::number(meanE));
        // collection
        QList<double> deaggVals = {psa, meanM, meanR, meanE};
        // append
        deagginfo_set.append({period, deaggVals});
    }

    if(deagginfo_set.empty()) {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("USGS Disaggregation Error: Failed to retrieve deaggregation results with the inputs provided");
        return deagginfo;
    }

    // deagginfo() asks only for the one period wanted, or the two periods bracketing Tx,
    // so the set is already what we need; just make sure it is ordered by period
    deagginfo = deagginfo_set;
    std::sort(deagginfo.begin(), deagginfo.end(),
              [](const QPair<double, QList<double>>& a, const QPair<double, QList<double>>& b) { return a.first < b.first; });

    return deagginfo;
}

QList<QPair<double, double>> NSHMPDeagg::getSpectrum(QJsonObject& json, double Tc, double Sac) const
{
    qDebug() << "Processing USGS deterministic GMPE results...";
    QList<QPair<double, double>> spectrum;
    QList<QPair<double, double>> cms; // conditional mean spectrum
    // json is the "response" object: { "means":  { "data": [ { "id": "BSSA_14", "data": { "pga": .., "sa": {"xs": [..], "ys": [..]} } } ] },
    //                                   "sigmas": { same layout } }
    auto meanSpectra = json["means"].toObject()["data"].toArray();
    auto sigmaSpectra = json["sigmas"].toObject()["data"].toArray();

    // Add an additional check of the return period range accepted by USGS
    if (returnPeriodBox->text().isEmpty() || returnPeriodBox->text().toInt() < 2)
    {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Please input a valid return period (2~9999).");
        return spectrum;
    }

    // append mean data and compute rho
    QList<QPair<double, double>> spectrum_rho;
    double eps = 0.0;
    for (auto curMean: meanSpectra)
    {
        auto xs = curMean.toObject()["data"].toObject()["sa"].toObject()["xs"].toArray();
        auto ys = curMean.toObject()["data"].toObject()["sa"].toObject()["ys"].toArray();

        for (int i=0; i<xs.size(); i++) {
            spectrum.append({xs[i].toDouble(),ys[i].toDouble()});
            spectrum_rho.append({xs[i].toDouble(),BakerJayaram2008(Tc,xs[i].toDouble())});
        }

        // back calculate epsilon step-1
        for (int i=0; i<xs.size()-1; i++) {
            if (xs[i].toDouble() <= Tc && xs[i+1].toDouble() >= Tc) {
                eps = log(Sac)-(log(ys[i].toDouble())+(log(Tc)-log(xs[i].toDouble()))*(log(ys[i+1].toDouble())-log(ys[i].toDouble()))/(log(xs[i+1].toDouble())-log(xs[i].toDouble())));
                qDebug() << "GMPE Sa(Tc) = " + QString::number(exp(log(ys[i].toDouble())+(log(Tc)-log(xs[i].toDouble()))*(log(ys[i+1].toDouble())-log(ys[i].toDouble()))/(log(xs[i+1].toDouble())-log(xs[i].toDouble()))));
                break;
            }
        }
    }

    QList<QPair<double, double>> spectrum_sigmas;
    for (auto curSigma: sigmaSpectra)
    {
        auto xs = curSigma.toObject()["data"].toObject()["sa"].toObject()["xs"].toArray();
        auto ys = curSigma.toObject()["data"].toObject()["sa"].toObject()["ys"].toArray();

        for (int i=0; i<xs.size(); i++) {
            spectrum_sigmas.append({xs[i].toDouble(),ys[i].toDouble()});
        }

        for (int i=0; i<xs.size()-1; i++) {
            if (xs[i].toDouble() <= Tc && xs[i+1].toDouble() >= Tc) {
                eps = eps/exp(log(ys[i].toDouble())+(log(Tc)-log(xs[i].toDouble()))*(log(ys[i+1].toDouble())-log(ys[i].toDouble()))/(log(xs[i+1].toDouble())-log(xs[i].toDouble())));
                break;
            }
        }

        qDebug() << "Back calculated epsilon = " + QString::number(eps);
    }

    if(spectrum.empty()) {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("USGS Disaggregation Error: Failed to retrieve uniform hazard spectrum with the inputs provided");
    }

    // compute cms
    for (int i=0; i<spectrum.size(); i++) {
        double curSa = exp(log(spectrum[i].second)+spectrum_sigmas[i].second*eps*spectrum_rho[i].second);
        cms.append({spectrum[i].first,curSa});
    }

    return cms;
}


QJsonObject NSHMPDeagg::serialize() const
{
    QJsonObject json;

    json["Vs30"] = vs30Box->currentText();
    json["Edition"] = editionBox->currentText();
    json["ReturnPeriod"] = returnPeriodBox->text();

    return json;
}

void NSHMPDeagg::deserialize(const QJsonObject &json)
{
    vs30Box->setCurrentText(json["Vs30"].toString());
    editionBox->setCurrentText(json["Edition"].toString());
    returnPeriodBox->setText(json["ReturnPeriod"].toString());

    double latitude, longitude;
    generalInfoWidget->getBuildingLocation(latitude, longitude);
    latitudeBox->setText(QString::number(latitude));
    longitudeBox->setText(QString::number(longitude));
}

QList<QPair<double, double>> NSHMPDeagg::spectrum() const
{

    // deagg first
    auto deagg_data = this->deagginfo();

    qDebug() << "USGS seismic disaggregation data processed.";

    QList<QList<QPair<double, double>>> spectrum_set;
    QList<QPair<double,double>> spectrum_tar;

    if (deagg_data.size()==0 || deagg_data.empty()) {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Error in USGS seismic disaggregation data - no target spectrum calculated");
        return spectrum_tar;
    }

    for (auto cur_deagg: deagg_data) {
        QString url(gmpeBox->currentData().toString());

        url.append("&mw=" + QString::number(cur_deagg.second[1]));
        url.append("&rjb=" + QString::number(cur_deagg.second[2]));
        url.append("&rrup=" + QString::number(cur_deagg.second[2]));
        url.append("&rx=" + QString::number(cur_deagg.second[2]));
        url.append("&vs30=" + vs30Box->currentData().toString());

        qDebug() << "USGS deterministic GMPE URL = " + url;

        QString errorMessage;
        QJsonObject replyJson = UsgsNshmp::getJson(networkManager, QUrl(url), errorMessage);
        QJsonObject response = replyJson["response"].toObject();

        if (!errorMessage.isEmpty() || !response.contains("means"))
        {
            qDebug() << "USGS deterministic GMPE results not valid." << errorMessage;
            const_cast<NSHMPDeagg*>(this)->emit statusUpdated("USGS GMM Spectra Error: " +
                (errorMessage.isEmpty() ? QString("unexpected reply from gmm service") : errorMessage));
            return QList<QPair<double, double>>();
        }

        spectrum_set.append(getSpectrum(response, cur_deagg.first, cur_deagg.second[0]));
    }

    if (spectrum_set.size()==1) {
        spectrum_tar = spectrum_set[0];
    } else {
        for (int i=0; i<spectrum_set[0].size(); i++) {
            spectrum_tar.append({spectrum_set[0][i].first,exp((log(spectrum_set[0][i].second)+log(spectrum_set[1][i].second))/2)});
        }
    }

    return spectrum_tar;
}

QList<QPair<double, QList<double>>> NSHMPDeagg::deagginfo() const
{
    //GoogleAnalytics::Report("RecordSelection", "USGS_DEAGG");

    // {model}/dynamic/disagg/{longitude}/{latitude}/{vs30}/{returnPeriod}?imt=X[&imt=Y]
    QStringList imts;
    if (imtBox->currentData().toString() == "SATX") {
        if (TxBox->text().isEmpty() || TxBox->text().toDouble() > availPeriod.last())
        {
            const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Please input a valid Tx (0.001~+"+ QString::number(availPeriod.last())+ ").");
            return QList<QPair<double, QList<double>>>();
        }
        auto TxString = QString::number(TxBox->text().toDouble());
        for (int i=0; i<availPeriod.size()-1; i++) {
            if (availPeriod[i]<=TxBox->text().toDouble() && availPeriod[i+1]>=TxBox->text().toDouble()) {
                // Tx is one of the available periods -> just that one,
                // otherwise the two bracketing periods (results are interpolated)
                if (availPeriod[i]==TxBox->text().toDouble()) {
                    imts << imtBox->itemData(i).toString();
                    break;
                }
                if (availPeriod[i+1]==TxBox->text().toDouble()) {
                    imts << imtBox->itemData(i+1).toString();
                    break;
                }
                imts << imtBox->itemData(i).toString() << imtBox->itemData(i+1).toString();
                break;
            }
        }
        if (imts.isEmpty()) {
            const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Please input a valid Tx (0.001~+"+ QString::number(availPeriod.last())+ ").");
            return QList<QPair<double, QList<double>>>();
        }
    } else {
        imts << imtBox->currentData().toString();
    }

    if (returnPeriodBox->text().isEmpty() || returnPeriodBox->text().toInt() < 2)
    {
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Please input a valid return period (2~9999).");
        return QList<QPair<double, QList<double>>>();
    }

    QString url(editionBox->currentData().toString());
    url.append("/disagg");
    url.append("/" + longitudeBox->text());
    url.append("/" + latitudeBox->text());
    url.append("/" + vs30Box->currentData().toString());
    url.append("/" + returnPeriodBox->text());
    url.append("?imt=" + imts.join("&imt="));

    qDebug() << "USGS sesimic disaggregation URL = " + url;
    const_cast<NSHMPDeagg*>(this)->emit statusUpdated("Running USGS seismic disaggregation (this can take 20s or more) ...");

    QString errorMessage;
    QJsonObject replyJson = UsgsNshmp::getJson(networkManager, QUrl(url), errorMessage);

    qDebug() << "USGS sesimic disaggregation results returned.";

    if (!errorMessage.isEmpty() || !replyJson["response"].toObject().contains("disaggs"))
    {
        qDebug() << "USGS sesimic disaggregation results not valid." << errorMessage;
        const_cast<NSHMPDeagg*>(this)->emit statusUpdated("USGS Disaggregation Error: " +
            (errorMessage.isEmpty() ? QString("unexpected reply from disaggregation service") : errorMessage));
        return QList<QPair<double, QList<double>>>();
    }

    return getDeagg(replyJson);
}

void NSHMPDeagg::SetComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}
