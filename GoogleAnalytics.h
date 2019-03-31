#ifndef GOOGLEANALYTICS_H
#define GOOGLEANALYTICS_H
#include <QUuid>
#include <QtNetwork/QNetworkAccessManager>

namespace GoogleAnalytics {
enum SessionControl {Start, End, None};

void Report(QString eventAction, QString category = "Action", SessionControl sessionControl = None);
void ReportStart();
void ReportLocalRun();
void ReportDesignSafeRun();
void StartSession();
void EndSession();
QUuid GetCommonClientId();
void SetTrackingId(QString trackingId);

static QString trackingId = "";
static QNetworkAccessManager networkManager;
}
#endif // GOOGLEANALYTICS_H
