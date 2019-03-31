#include "GoogleAnalytics.h"
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QCoreApplication>
#include <QSettings>

namespace GoogleAnalytics {

void ReportStart()
{
    Report("Start");
}

void ReportLocalRun()
{
    Report("Local", "Simulation");
}

void ReportDesignSafeRun()
{
    Report("DesignSafe", "Simulation");
}

void StartSession()
{
    Report("Start", "Session", SessionControl::Start);

}

void EndSession()
{
    Report("End", "Session", SessionControl::End);
}

//TODO: This code may need to be refactored and shared in SimCenterCommon
QUuid GetCommonClientId()
{
    QSettings commonSettings("SimCenter", "Common"); //These names will need to be constants to be shared
    QVariant clientIdSetting = commonSettings.value("clientId");
    if (!clientIdSetting.isValid())
    {
        commonSettings.setValue("clientId", QUuid::createUuid());
        clientIdSetting = commonSettings.value("clientId");
    }
    return clientIdSetting.toUuid();
}

void SetTrackingId(QString atrackingId)
{
    trackingId = atrackingId;
}

void Report(QString eventAction, QString category, SessionControl sessionControl)
{
    QNetworkRequest request;
    QUrl host("http://www.google-analytics.com/collect");
    request.setUrl(host);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    // setup parameters of request
    QString requestParams;
    QUuid clientId = GetCommonClientId();
    requestParams += "v=1"; // version of protocol
    requestParams += "&tid=" + trackingId; // Google Analytics account
    requestParams += "&cid=" + clientId.toString(); // unique user identifier
    requestParams += "&t=event";  // hit type = event others pageview, exception
    requestParams += "&an=" + QCoreApplication::applicationName();   // app name
    requestParams += "&av=" + QCoreApplication::applicationVersion(); // app version
    requestParams += "&ec=" + category;   // event category
    requestParams += "&ea=" + eventAction; // event action
    requestParams += "&aip=1"; // Anonymize IP
    requestParams += "&ds=app"; // DataSource

    if(sessionControl == SessionControl::Start) {
        requestParams += "&sc=start"; // Start Session
    }
    else if (sessionControl == SessionControl::End){
        requestParams += "&sc=end"; // End Session
    }
    // send request via post method
    QNetworkReply* reply = networkManager.post(request, requestParams.toStdString().c_str());

    //If this a session end, we have to wait for the network request to finish
    if (sessionControl == SessionControl::End)
    {
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }
}

}
