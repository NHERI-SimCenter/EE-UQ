#include "PeerNgaWest2Client.h"

#include <QDebug>


#include <QRegularExpression>
#include <QFile>
#include <QStandardPaths>
#include <QMainWindow>
#include <QApplication>
#include <QStatusBar>
#include <QDir>
#include<QHttpMultiPart>
#include <QSslConfiguration>

PeerNgaWest2Client::PeerNgaWest2Client(QObject *parent) : QObject(parent),
    nRecords(3), isLoggedIn(false), retries(0)
{
    QNetworkCookie cookie("sourceDb_flag", "1");
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);

    searchScaleFlag = -1;

    setupConnection();
}

bool PeerNgaWest2Client::loggedIn()
{
    return this->isLoggedIn;
}

void PeerNgaWest2Client::signIn(QString username, QString password)
{
    emit statusUpdated("Logging in to PEER NGA West 2 Database");
    //TODO: storing password in memory may not be the best choice for security
    //a more secure way of handling password might be needed
    this->username = username;
    this->password = password;

    QNetworkRequest peerSignInPageRequest(QUrl("https://ngawest2.berkeley.edu/users/sign_in"));
    signInPageReply = networkManager.get(peerSignInPageRequest);
}

void PeerNgaWest2Client::selectRecords(QList<QPair<double, double>> spectrum, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range)
{
    emit selectionStarted();
    emit statusUpdated("Performing Record Selection...");

    this->nRecords = nRecords;
    this->magnitudeRange = magnitudeRange;
    this->distanceRange = distanceRange;
    this->vs30Range = vs30Range;

    uploadFileRequest.setUrl(QUrl("https://ngawest2.berkeley.edu/spectras/uploadFile"));
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //Token part
    QHttpPart tokenPart;
    tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"authenticity_token\""));
    tokenPart.setBody(authenticityToken.toUtf8());
    multiPart->append(tokenPart);

    //Spectrum File part
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"upload[datafile]\"; filename=\"UserSpectrum.csv\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/vnd.ms-excel"));

    QString userSpectrum;
    userSpectrum.append("User Defined Spectrum\r\n");
    userSpectrum.append("\r\n");
    userSpectrum.append("T (s),Sa (g)\r\n");

    for(auto& point: spectrum)
        userSpectrum.append(QString::number(point.first) + ',' + QString::number(point.second) + "\r\n");

    filePart.setBody(userSpectrum.toUtf8());
    multiPart->append(filePart);

    //Commit part
    QHttpPart commitPart;
    commitPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"commit\""));
    commitPart.setBody("Upload");
    multiPart->append(commitPart);

    recordsToDownload.clear();
    uploadFileReply = networkManager.post(uploadFileRequest, multiPart);
}

void PeerNgaWest2Client::selectRecords(double sds, double sd1, double tl, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range, int peerSRkey)
{
    emit selectionStarted();
    emit statusUpdated("Performing Record Selection...");

    this->nRecords = nRecords;
    this->SRkey = peerSRkey;

    QNetworkCookie cookie("SpectrumModel_Dropdown", "99");
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);

    postSpectraRequest.setUrl(QUrl("https://ngawest2.berkeley.edu/spectras"));
    postSpectraRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    postSpectraParameters.clear();
    postSpectraParameters.addQueryItem("authenticity_token", authenticityToken);
    postSpectraParameters.addQueryItem("spectra[NGAInputData_NGAModelSelection]", "99");//99 is design spectrum
    postSpectraParameters.addQueryItem("model[AS]", "0");
    postSpectraParameters.addQueryItem("model[BA]", "0");
    postSpectraParameters.addQueryItem("model[CY]", "0");
    postSpectraParameters.addQueryItem("model[CB]", "0");
    postSpectraParameters.addQueryItem("model[ID]", "0");
    postSpectraParameters.addQueryItem("spectra[NGAInputData_Sds]", QString::number(sds));
    postSpectraParameters.addQueryItem("spectra[NGAInputData_Sd1]", QString::number(sd1));
    postSpectraParameters.addQueryItem("spectra[NGAInputData_TL]", QString::number(tl));
    postSpectraParameters.addQueryItem("spectra[menu_Mechanism]", "1");

    this->magnitudeRange = magnitudeRange;
    this->distanceRange = distanceRange;
    this->vs30Range = vs30Range;

    recordsToDownload.clear();
    postSpectraReply = networkManager.post(postSpectraRequest, postSpectraParameters.query().toUtf8());
}

void PeerNgaWest2Client::selectRecords(QStringList records)
{
    emit selectionStarted();
    emit statusUpdated("Performing Record Selection...");

    QNetworkCookie cookie("SpectrumModel_Dropdown", "88");
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);

    postSpectraRequest.setUrl(QUrl("https://ngawest2.berkeley.edu/spectras"));
    postSpectraRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    postSpectraParameters.clear();
    postSpectraParameters.addQueryItem("authenticity_token", authenticityToken);
    postSpectraParameters.addQueryItem("spectra[NGAInputData_NGAModelSelection]", "88");//88 no spectrum
    postSpectraParameters.addQueryItem("model[AS]", "0");
    postSpectraParameters.addQueryItem("model[BA]", "0");
    postSpectraParameters.addQueryItem("model[CY]", "0");
    postSpectraParameters.addQueryItem("model[CB]", "0");
    postSpectraParameters.addQueryItem("model[ID]", "0");
    postSpectraParameters.addQueryItem("spectra[menu_Mechanism]", "1");

    recordsToDownload = records;
    postSpectraReply = networkManager.post(postSpectraRequest, postSpectraParameters.query().toUtf8());
}

void PeerNgaWest2Client::setupConnection()
{
    QObject::connect(&networkManager, &QNetworkAccessManager::finished, this, &PeerNgaWest2Client::processNetworkReply);
}

void PeerNgaWest2Client::processNetworkReply(QNetworkReply *reply)
{    
    /*
       qDebug() << "Processing reply";

       qDebug() << "Default SSL configuration isNull: "
                << QSslConfiguration::defaultConfiguration().isNull();

       qDebug() << "SSL configuration used by QNAM isNull: "
                << reply->sslConfiguration().isNull();
       qDebug() << QSslSocket::sslLibraryBuildVersionString();

       qDebug() << "support ssl: " << QSslSocket::supportsSsl();
  */
    if(reply == signInPageReply)
        processSignInPageReply();

    else if(reply == signInReply)
        processSignInReply();

    else if(reply == uploadFileReply)
        processUploadFileReply();

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

    if(signInPageReply->error() == QNetworkReply::HostNotFoundError)
    {
        emit statusUpdated("Failed to connect to PEER NGA West 2, Please check your internet connection");
        emit loginFinished(false);
        return;
    }

#ifdef _WIN32
    // mac reg expression not working with \" char .. improvise!
    QRegularExpression regex("authenticity_token.+div");
    auto match = regex.match(QString(signInPageReply->readAll()));
    if (match.hasMatch()) {
        QString stringCaptured = match.captured(0);
        QString startString = stringCaptured.remove(0,41);
        QString finalString = startString.remove(44,100);
        authenticityToken = finalString;

    } else {
        qDebug() << "NO MATCH\n";
    }

#else
    QRegularExpression regex("[\\s\\S]+authenticity_token.+value=\"(.+)\".+<\\/div>");
    auto match = regex.match(QString(signInPageReply->readAll()));
    if (match.hasMatch()) {

    } else {
        qDebug() << "NO MATCH\n";
    }

    authenticityToken = match.captured(1);
#endif

    peerSignInRequest.setUrl(QUrl("https://ngawest2.berkeley.edu/users/sign_in"));
    peerSignInRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    signInParameters.clear();
    signInParameters.addQueryItem("authenticity_token", authenticityToken);
    signInParameters.addQueryItem("user[email]", username);
    signInParameters.addQueryItem("user[password]", password);
    signInParameters.addQueryItem("user[remember_me]", "0");
    signInParameters.addQueryItem("commit", "Sign in");

    signInReply = networkManager.post(peerSignInRequest, signInParameters.query().toUtf8());
}


static void debugRequest(QNetworkRequest request, QByteArray data = QByteArray())
{
  qDebug() << request.url().toString();
  const QList<QByteArray>& rawHeaderList(request.rawHeaderList());
  foreach (QByteArray rawHeader, rawHeaderList) {
    qDebug() << request.rawHeader(rawHeader);
  }
  qDebug() << data;
}

void PeerNgaWest2Client::processSignInReply()
{
    if(signInReply->error() == QNetworkReply::NoError)
    {
        //Checking if we are being redirected
        //This means log in is sucessful
        if(signInReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
        {
            emit statusUpdated("Sucessfully signed in to PEER NGA West 2");
            isLoggedIn = true;
            emit loginFinished(true);

            if(retries > 0 && retries <= 5)
            {
                retryPostSpectra();
            }
            return;
        }
    }

    auto err = signInReply->error();

    if(signInReply->error() == QNetworkReply::HostNotFoundError)
        emit statusUpdated("Failed to connect to PEER NGA West 2, Please check your internet connection");
    else
        emit statusUpdated("Failed to login to PEER NGA West 2 database, please check your credentials!");

    emit loginFinished(false);
    isLoggedIn = false;

}

void PeerNgaWest2Client::processUploadFileReply()
{
     qDebug() << "PROCESS UPLOAD FILE REPLY";

    QNetworkCookie cookie("SpectrumModel_Dropdown", "0");
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);

    postSpectraRequest.setUrl(QUrl("https://ngawest2.berkeley.edu/spectras"));
    postSpectraRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    postSpectraParameters.clear();
    postSpectraParameters.addQueryItem("authenticity_token", authenticityToken);
    postSpectraParameters.addQueryItem("spectra[NGAInputData_NGAModelSelection]", "0");//0 user spectrum
    postSpectraParameters.addQueryItem("model[AS]", "0");
    postSpectraParameters.addQueryItem("model[BA]", "0");
    postSpectraParameters.addQueryItem("model[CY]", "0");
    postSpectraParameters.addQueryItem("model[CB]", "0");
    postSpectraParameters.addQueryItem("model[ID]", "0");
    postSpectraParameters.addQueryItem("spectra[menu_Mechanism]", "1");

    for (auto cookie: networkManager.cookieJar()->cookiesForUrl(QUrl("https://ngawest2.berkeley.edu")))
        if (0 == cookie.name().compare("upload_file"))
            postSpectraParameters.addQueryItem("spectra[filename]", cookie.value());


    //debugRequest(postSpectraRequest, postSpectraParameters.query().toUtf8());
    postSpectraReply = networkManager.post(postSpectraRequest, postSpectraParameters.query().toUtf8());
}

void PeerNgaWest2Client::processPostSpectrumReply()
{
    qDebug() << "PROCESS POST SPECTRUIM REPLY";

    QByteArray data = postSpectraReply->readAll();
    if(postSpectraReply->error() != QNetworkReply::NoError)
    {
        //if it fails we will retry
        retry();
        return;
    }
    else
    {
        int statusCode = postSpectraReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        //if we get redirected to sign in we will retry
        if(statusCode == 302) {
            QUrl redirectUrl = postSpectraReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (0 == redirectUrl.toString().compare("/users/sign_in?unauthenticated=true"))
            {
                retry();
                return;
            }
        }

        retries = 0;
        auto url = postSpectraReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        auto searchPost = url.toString().remove("/new").remove("/edit").remove("/searches").append("/searches");

        QNetworkRequest searchPostRequest(searchPost);
        searchPostRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrlQuery params;
        params.addQueryItem("authenticity_token", authenticityToken);
        params.addQueryItem("search[DampingRatio]", "0.05");
        params.addQueryItem("search[SRkey]", QString::number(this->SRkey));
        params.addQueryItem("search[search_station_name]", "");
        params.addQueryItem("search[search_eq_name]", "");

        qDebug() << "RECORDS TO DOWNLOAD: " << recordsToDownload;

        if (recordsToDownload.length() == 0) {

            // it is  a search, provide selection quantaties
            params.addQueryItem("search[search_nga_number]", "");


            if(searchScaleFlag == 0)
            {
                params.addQueryItem("search[scale_flag]", "0");
            }
            else if(searchScaleFlag == 1)
            {
                params.addQueryItem("search[scale_flag]", "1");
                params.addQueryItem("search[period]", searchPeriodPoints);
                params.addQueryItem("search[weight]", searchWeightPoints);
            }
            else if(searchScaleFlag == 2)
            {
                params.addQueryItem("search[scale_flag]", "2");
                params.addQueryItem("search[SinglePeriodScalingT]", searchSinglePeriodScalingT);
                params.addQueryItem("search[period]", searchPeriodPoints);
                params.addQueryItem("search[weight]", searchWeightPoints);
            }

            params.addQueryItem("search[output_num]", QString::number(nRecords));

            if(magnitudeRange.isValid() && !magnitudeRange.isNull())
            {
                auto mangnitudePair = magnitudeRange.value<QPair<double, double>>();
                QString magnitude = QString::number(mangnitudePair.first) + ',' + QString::number(mangnitudePair.second);
                params.addQueryItem("search[magnitude]", magnitude);
            }

            if(distanceRange.isValid() && !distanceRange.isNull())
            {
                auto distancePair = distanceRange.value<QPair<double, double>>();
                QString rrup = QString::number(distancePair.first) + ',' + QString::number(distancePair.second);
                params.addQueryItem("search[rrup]", rrup);
            }

            if(vs30Range.isValid() && !vs30Range.isNull())
            {
                auto vs30Pair = vs30Range.value<QPair<double, double>>();
                QString vs30 = QString::number(vs30Pair.first) + ',' + QString::number(vs30Pair.second);
                params.addQueryItem("search[vs30]", vs30);
            }
        } else {

            // just want to download specific records
            QString list;
            int lengthRecords = recordsToDownload.length();
            for (int i=0; i<lengthRecords; i++) {
                list.append(recordsToDownload.at(i));
                if (i != lengthRecords-1)
                    list.append(",");
            }

            qDebug() << "Peer List: " << list;

            params.addQueryItem("search[search_nga_number]", list);
            params.addQueryItem("search[scale_flag]", "0");
            params.addQueryItem("search[faultType]", "1");
            params.addQueryItem("search[magnitude]", "");
            params.addQueryItem("search[rjb]", "");
            params.addQueryItem("search[rrup]", "");
            params.addQueryItem("search[vs30]", "");
            params.addQueryItem("search[duration]", "");
            params.addQueryItem("search[pulse]", "1");
            params.addQueryItem("search[Pulse]", "1");
            params.addQueryItem("search[synth_FaultType]", "0");
            params.addQueryItem("search[synth_Adjacency]", "0");
            params.addQueryItem("search[SRmeanFlag]", "0");
            params.addQueryItem("search[vs30]", "");
            params.addQueryItem("search[rrup]", "");
            params.addQueryItem("search[magnitude]", "");
        }

        debugRequest(searchPostRequest, params.query().toUtf8());
        postSearchReply = networkManager.post(searchPostRequest, params.query().toUtf8());
    }
}

void PeerNgaWest2Client::processPostSearchReply()
{
     qDebug() << "PROCESS POST SEARCH REPLY";
    emit statusUpdated("Retrieving Record Selection Results from PEER NGA West 2 Database");

    auto url = postSearchReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    auto getRecordsUrl = url.toString().replace("/edit", "/?getRecords=1");



    QNetworkRequest getRecordsRequest(getRecordsUrl);
    qDebug() << "URL: " << getRecordsRequest.url().toString();
    getRecordsReply = networkManager.get(getRecordsRequest);
}

void PeerNgaWest2Client::processGetRecordsReply()
{

    emit statusUpdated("Downloading Ground Motions from PEER NGA West 2 Database");
    auto replyText = QString(getRecordsReply->readAll());
    auto url = replyText.remove("window.location.href = \"").remove("\";").prepend("https://ngawest2.berkeley.edu");

    QNetworkRequest downloadRecordsRequest(url);
    downloadRecordsReply = networkManager.get(downloadRecordsRequest);
}


void PeerNgaWest2Client::processDownloadRecordsReply()
{
     qDebug() << "PROCESS DOWNLOADS RECORD REPLY";
    emit selectionFinished();
    auto tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    if(!QDir(tempLocation).exists())
    {
        if(!QDir(tempLocation).mkpath("."))
        {
            emit statusUpdated("Ground Motions Download Failed!");
        }
    }
    //TODO: we might need to use temporary files
    QString recordsPath = tempLocation.append("/PeerRecords.zip");

    qDebug() << "RECORDS PATH: " << recordsPath;

    QFile file(recordsPath);
    if(!file.open(QIODevice::WriteOnly))
    {
        emit statusUpdated("Ground Motions Download Failed!");
        return;
    }

    file.write(downloadRecordsReply->readAll());
    file.close();

    emit statusUpdated("Ground Motions Downloaded Sucessfully");
    emit recordsDownloaded(recordsPath);
}

void PeerNgaWest2Client::retryPostSpectra()
{
    postSpectraParameters.removeQueryItem("authenticity_token");
    postSpectraParameters.addQueryItem("authenticity_token", authenticityToken);
    postSpectraReply = networkManager.post(postSpectraRequest, postSpectraParameters.query().toUtf8());
}

void PeerNgaWest2Client::retrySignIn()
{
    QNetworkRequest peerSignInPageRequest(QUrl("https://ngawest2.berkeley.edu/users/sign_in"));
    signInPageReply = networkManager.get(peerSignInPageRequest);
}

void PeerNgaWest2Client::retry()
{
    if(retries < 5)
    {
        retries++;
        emit statusUpdated("Failed to submit target spectrum to PEER NGA West 2 Database, retrying");
        retrySignIn();
    }
    else
    {
        emit statusUpdated("Failed to submit target spectrum to PEER NGA West 2 Database after 5 retries, Please try again shortly.");
        retries = 0;
        emit selectionFinished();
        retrySignIn();
    }
}


void PeerNgaWest2Client::setScalingParameters(const int scaleFlag,
                                              const QString& periodPoints,
                                              const QString& weightPoints,
                                              const QString& scalingPeriod)
{
    searchScaleFlag = scaleFlag;
    searchPeriodPoints = periodPoints;
    searchWeightPoints = weightPoints;
    searchSinglePeriodScalingT = scalingPeriod;
}
