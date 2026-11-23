#ifndef USGS_NSHMP_SERVICE_H
#define USGS_NSHMP_SERVICE_H

//
// Helpers for the USGS NSHMP web services (https://earthquake.usgs.gov/ws/nshmp/...)
// used by NSHMPTarget (hazard curves -> UHS) and NSHMPDeagg (disaggregation + GMM spectra).
//
// The old nshmp-haz-ws / hazws services were retired by USGS (they now return the HTML
// of the Hazard Toolbox web app). The new services redirect unversioned model names
// (e.g. conus-2023) to the current revision (conus-2023.R2) but the redirect is to
// http://, which Qt refuses to follow from https://. So redirects are followed here by
// hand, forcing https.
//

#include <QString>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

namespace UsgsNshmp {

static const QString baseUrl("https://earthquake.usgs.gov/ws/nshmp");

// GET url and return the reply as JSON. On failure returns an empty object and sets errorMessage.
inline QJsonObject getJson(QNetworkAccessManager& networkManager, QUrl url, QString& errorMessage)
{
    errorMessage.clear();

    for (int redirects = 0; redirects < 5; redirects++) {

        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
        request.setTransferTimeout(180000); // disaggregation can take ~20s+ on the USGS side

        QNetworkReply* reply = networkManager.get(request);
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        QByteArray data = reply->readAll();
        QNetworkReply::NetworkError error = reply->error();
        QString errorString = reply->errorString();
        reply->deleteLater();

        if (status >= 300 && status < 400 && redirect.isValid()) {
            url = url.resolved(redirect);
            if (url.scheme() == "http")
                url.setScheme("https");
            continue;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (doc.isObject()) {
            QJsonObject json = doc.object();
            // service level errors come back as {"status":"error","response":"<message>"}
            if (json["status"].toString() == "error") {
                errorMessage = "USGS service error: " + json["response"].toString();
                return QJsonObject();
            }
            return json;
        }

        if (error != QNetworkReply::NoError)
            errorMessage = "USGS service request failed: " + errorString;
        else
            errorMessage = "USGS service did not return JSON (the service may have moved): " + url.toString();
        return QJsonObject();
    }

    errorMessage = "USGS service: too many redirects for " + url.toString();
    return QJsonObject();
}

// "SA0P75" -> 0.75 ; anything else (PGA, PGV) -> -1
inline double imtPeriod(QString imt)
{
    if (!imt.startsWith("SA"))
        return -1.0;
    return imt.remove("SA").replace('P', '.').toDouble();
}

} // namespace UsgsNshmp

#endif // USGS_NSHMP_SERVICE_H
