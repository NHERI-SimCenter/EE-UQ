#ifndef PEERNGAWEST2CLIENT_H
#define PEERNGAWEST2CLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QStringList>

class PeerNgaWest2Client : public QObject
{
    Q_OBJECT
public:
    explicit PeerNgaWest2Client(QObject *parent = nullptr);
    bool loggedIn();

    // Login is done interactively in a QWebEngineView (PeerLoginDialog) because the
    // site is protected by Cloudflare Turnstile; the dialog hands the resulting
    // session cookies + Rails CSRF token to this client, which then issues requests as before.
    void setSession(const QList<QNetworkCookie>& cookies, const QString& csrfToken);
    void invalidateSession();

    // re-post the last spectrum request (e.g. after logging in again on sessionExpired)
    void resubmit();
    void selectRecords(double sds, double sd1, double tl, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range, QVariant durationRange, int peerSRkey, int peerSRmeanFlag, int peerFaultType, int peerPulse);
    void selectRecords(QList<QPair<double, double>> spectrum, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range, QVariant durationRange, int peerSRkey, int peerSRmeanFlag, int peerFaultType, int peerPulse);
    void selectRecords(QStringList);

    void setScalingParameters(const int scaleFlag,
                              const QString& periodPoints,
                              const QString& weightPoints,
                              const QString& scalingPeriod);

signals:
    void loginFinished(bool result);
    void sessionExpired();
    void recordsDownloaded(QString recordsPath);
    void statusUpdated(QString status);
    void selectionStarted();
    void selectionFinished();

public slots:

private:
    QNetworkAccessManager networkManager;

    QNetworkReply* getSpectraReply;
    QNetworkReply* postSpectraReply;
    QNetworkReply* postSearchReply;
    QNetworkReply* getRecordsReply;
    QNetworkReply* downloadRecordsReply;
    QNetworkReply* uploadFileReply;

    QString authenticityToken;
    int nRecords;
    int SRkey;
    int SRmeanFlag;
    int faultType;
    int pulse;
    bool isLoggedIn;
    QVariant magnitudeRange;
    QVariant distanceRange;
    QVariant vs30Range;
    QVariant durationRange;

    int searchScaleFlag;
    QString searchPeriodPoints;
    QString searchWeightPoints;
    QString searchSinglePeriodScalingT;

    //Data for retry on failure
    int retries;
    QNetworkRequest postSpectraRequest;
    QUrlQuery postSpectraParameters;
    QNetworkRequest uploadFileRequest;
    QStringList recordsToDownload;

    void setupConnection();
    void processNetworkReply(QNetworkReply *reply);

    void processUploadFileReply();
    void processPostSpectrumReply();
    void processPostSearchReply();
    void processGetRecordsReply();
    void processDownloadRecordsReply();
    void retryPostSpectra();
    void retry();


};

#endif // PEERNGAWEST2CLIENT_H
