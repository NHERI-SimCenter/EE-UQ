#ifndef PEERNGAWEST2CLIENT_H
#define PEERNGAWEST2CLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class PeerNgaWest2Client : public QObject
{
    Q_OBJECT
public:
    explicit PeerNgaWest2Client(QObject *parent = nullptr);
    bool loggedIn();
    void signIn(QString username, QString password);
    void selectRecords(double sds, double sd1, double tl, int nRecords);

signals:
    void loginFinished(bool result);
    void recordsDownloaded(QString recordsPath);

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

    QString authenticityToken;
    QString username;
    QString password;
    int nRecords;
    bool isLoggedIn;

    void setupConnection();
    void processNetworkReply(QNetworkReply *reply);

    void processSignInPageReply();
    void processSignInReply();
    void processPostSpectrumReply();
    void processPostSearchReply();
    void processGetRecordsReply();
    void processDownloadRecordsReply();

};

#endif // PEERNGAWEST2CLIENT_H
