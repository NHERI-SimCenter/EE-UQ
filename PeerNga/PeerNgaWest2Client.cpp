#include "PeerNgaWest2Client.h"
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QDebug>
#include <QUrl>
#include <QUrlQuery>

#include <QRegularExpression>
#include <QFile>
#include <QStandardPaths>

PeerNgaWest2Client::PeerNgaWest2Client(QObject *parent) : QObject(parent),
    nRecords(3), isLoggedIn(false)
{
    QNetworkCookie cookie("sourceDb_flag", "1");
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);

    setupConnection();
}

bool PeerNgaWest2Client::loggedIn()
{
    return this->isLoggedIn;
}

void PeerNgaWest2Client::signIn(QString username, QString password)
{
    //TODO: storing password in memeory may not be the best choice for security
    //a more secure way of handling password might be needed
    this->username = username;
    this->password = password;

    QNetworkRequest peerSignInPageRequest(QUrl("https://ngawest2.berkeley.edu/users/sign_in"));
    signInPageReply = networkManager.get(peerSignInPageRequest);
}

void PeerNgaWest2Client::selectRecords(double sds, double sd1, double tl, int nRecords)
{
    this->nRecords = nRecords;

    QNetworkCookie cookie("SpectrumModel_Dropdown", "99");
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);

    QNetworkRequest postSpectraRequest(QUrl("https://ngawest2.berkeley.edu/spectras"));
    postSpectraRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("authenticity_token", authenticityToken);
    params.addQueryItem("spectra[NGAInputData_NGAModelSelection]", "99");//99 is design spectrum
    params.addQueryItem("model[AS]", "0");
    params.addQueryItem("model[BA]", "0");
    params.addQueryItem("model[CY]", "0");
    params.addQueryItem("model[CB]", "0");
    params.addQueryItem("model[ID]", "0");
    params.addQueryItem("spectra[NGAInputData_Sds]", QString::number(sds));
    params.addQueryItem("spectra[NGAInputData_Sd1]", QString::number(sd1));
    params.addQueryItem("spectra[NGAInputData_TL]", QString::number(tl));
    params.addQueryItem("spectra[menu_Mechanism]", "1");

    postSpectraReply = networkManager.post(postSpectraRequest, params.query().toUtf8());
}

void PeerNgaWest2Client::setupConnection()
{
    QObject::connect(&networkManager, &QNetworkAccessManager::finished, this, &PeerNgaWest2Client::processNetworkReply);
}

void PeerNgaWest2Client::processNetworkReply(QNetworkReply *reply)
{
    qDebug() <<  "Request " << reply->request().url() <<  " replied with the result" << reply->error();

    qDebug() <<  "Cookies: " << networkManager.cookieJar()->cookiesForUrl(QUrl("https://ngawest2.berkeley.edu")).count();

    for (auto cookie: networkManager.cookieJar()->cookiesForUrl(QUrl("https://ngawest2.berkeley.edu")))
    {
        qDebug() << cookie.name() << ": " << cookie.value();
    }

    if(reply == signInPageReply)
        processSignInPageReply();

    else if(reply == signInReply)
        processSignInReply();

    else if(reply == postSpectraReply)
        processPostSpectrumReply();

    else if(reply == postSearchReply)
        processPostSearchReply();

    else if(reply == getRecordsReply)
        processGetRecordsReply();

    else if(reply == downloadRecordsReply)
        processDownloadRecordsReply();

    else
        qDebug() << reply->readAll();

}

void PeerNgaWest2Client::processSignInPageReply()
{
    QRegularExpression regex("[\\s\\S]+authenticity_token.+value=\"(.+)\".+<\\/div>");

    auto match = regex.match(QString(signInPageReply->readAll()));
    authenticityToken = match.captured(1);
    qDebug() << "authenticity_token: " << ": " << authenticityToken;

    QNetworkRequest peerSignInRequest(QUrl("https://ngawest2.berkeley.edu/users/sign_in"));
    peerSignInRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery params;
    params.addQueryItem("authenticity_token", authenticityToken);
    params.addQueryItem("user[email]", username);
    params.addQueryItem("user[password]", password);
    params.addQueryItem("user[remember_me]", "0");
    params.addQueryItem("commit", "Sign in");

    signInReply = networkManager.post(peerSignInRequest, params.query().toUtf8());
}

void PeerNgaWest2Client::processSignInReply()
{
    if(signInReply->error() == QNetworkReply::NoError)
    {
        //Checking if we are being redirected
        //This means log in is sucessful
        if(signInReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
        {
            isLoggedIn = true;
            emit loginFinished(true);
            return;
        }
    }

    emit loginFinished(false);
    isLoggedIn = false;

}

void PeerNgaWest2Client::processPostSpectrumReply()
{
    if(postSpectraReply->error() != QNetworkReply::NoError)
    {
        QNetworkRequest peerSignInPageRequest(QUrl("https://ngawest2.berkeley.edu/users/sign_in"));
        signInPageReply = networkManager.get(peerSignInPageRequest);
    }
    else
    {
        qDebug() << postSpectraReply->readAll();
        auto url = postSpectraReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        auto searchPost = url.toString().remove("/new").remove("/edit").append("/searches");

        QNetworkRequest searchPostRequest(searchPost);
        searchPostRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrlQuery params;

        params.addQueryItem("authenticity_token", authenticityToken);
        params.addQueryItem("search[search_nga_number]", "");
        params.addQueryItem("search[DampingRatio]", "0.05");
        params.addQueryItem("search[SRkey]", "1");
        params.addQueryItem("search[search_station_name]", "");
        params.addQueryItem("search[search_eq_name]", "");
        params.addQueryItem("search[scale_flag]", "1");
        params.addQueryItem("search[period]", "0.1,1,10.0");
        params.addQueryItem("search[weight]", "1.0,1.0,1.0");
        params.addQueryItem("search[output_num]", QString::number(nRecords));

        postSearchReply = networkManager.post(searchPostRequest, params.query().toUtf8());
        qDebug() << searchPost;
    }
}

void PeerNgaWest2Client::processPostSearchReply()
{
    auto url = postSearchReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    auto getRecordsUrl = url.toString().replace("/edit", "/?getRecords=1");
    qDebug() << url;
    QNetworkRequest getRecordsRequest(getRecordsUrl);
    getRecordsReply = networkManager.get(getRecordsRequest);
}

void PeerNgaWest2Client::processGetRecordsReply()
{
    auto replyText = QString(getRecordsReply->readAll());
    qDebug() << replyText;
    auto url = replyText.remove("window.location.href = \"").remove("\";").prepend("https://ngawest2.berkeley.edu");
    qDebug() << url;

    QNetworkRequest downloadRecordsRequest(url);
    downloadRecordsReply = networkManager.get(downloadRecordsRequest);
}

void PeerNgaWest2Client::processDownloadRecordsReply()
{
    auto cacheLocation = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);

    //TODO: we might need to use temporary files
    QString recordsPath = cacheLocation.append("/PeerRecords.zip");
    QFile file(recordsPath);
    file.open(QIODevice::WriteOnly);
    file.write(downloadRecordsReply->readAll());
    file.close();
    emit recordsDownloaded(recordsPath);
}
