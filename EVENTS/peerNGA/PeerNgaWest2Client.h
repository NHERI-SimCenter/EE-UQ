#ifndef PEERNGAWEST2CLIENT_H
#define PEERNGAWEST2CLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkCookieJar>
#include <QNetworkCookie>

class PeerNgaWest2Client : public QObject
{
    Q_OBJECT
public:
    explicit PeerNgaWest2Client(QObject *parent = nullptr);
    bool loggedIn();
    void signIn(QString username, QString password);
    void selectRecords(double sds, double sd1, double tl, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range);
    void selectRecords(QList<QPair<double, double>> spectrum, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range);

    void setScalingParameters(const int scaleFlag,
                              const QString& periodPoints,
                              const QString& weightPoints,
                              const QString& scalingPeriod);

signals:
    void loginFinished(bool result);
    void recordsDownloaded(QString recordsPath);
    void statusUpdated(QString status);
    void selectionStarted();
    void selectionFinished();

public slots:

private:
    QNetworkAccessManager networkManager;

    QNetworkReply* signInPageReply;
    QNetworkReply* signInReply;
    QNetworkReply* getSpectraReply;
    QNetworkReply* postSpectraReply;
    QNetworkReply* postSearchReply;
    QNetworkReply* getRecordsReply;
    QNetworkReply* downloadRecordsReply;
    QNetworkReply* uploadFileReply;

    QString authenticityToken;
    QString username;
    QString password;
    int nRecords;
    bool isLoggedIn;
    QVariant magnitudeRange;
    QVariant distanceRange;
    QVariant vs30Range;

    int searchScaleFlag;
    QString searchPeriodPoints;
    QString searchWeightPoints;
    QString searchSinglePeriodScalingT;

    //Data for retry on failure
    int retries;
    QList<QNetworkCookie> signInCookies;
    QNetworkRequest postSpectraRequest;
    QUrlQuery postSpectraParameters;
    QNetworkRequest peerSignInRequest;
    QUrlQuery signInParameters;
    QNetworkRequest uploadFileRequest;

    void setupConnection();
    void processNetworkReply(QNetworkReply *reply);

    void processSignInPageReply();
    void processSignInReply();
    void processUploadFileReply();
    void processPostSpectrumReply();
    void processPostSearchReply();
    void processGetRecordsReply();
    void processDownloadRecordsReply();
    void retryPostSpectra();
    void retrySignIn();
    void retry();


};

#endif // PEERNGAWEST2CLIENT_H
