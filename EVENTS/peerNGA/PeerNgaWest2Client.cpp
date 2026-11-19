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

static const QString peerHost("https://ngawest2.berkeley.edu");

//
// DEBUG helpers - log every request sent and reply received
// uncomment the next line to see all PEER http requests/replies in the console
//

#define PEER_NGA_DEBUG

[[maybe_unused]] static QString shortValue(const QByteArray& value)
{
    // don't dump full session secrets to the console
    return value.size() > 16 ? QString::fromUtf8(value.left(16)) + "..." : QString::fromUtf8(value);
}

static void logRequest(const char* method, QNetworkAccessManager& manager,
                       const QNetworkRequest& request, const QByteArray& body = QByteArray())
{
#ifndef PEER_NGA_DEBUG
    Q_UNUSED(method); Q_UNUSED(manager); Q_UNUSED(request); Q_UNUSED(body);
#else
    qDebug().noquote() << "PEER >>" << method << request.url().toString();

    QStringList cookieList;
    for (const auto& cookie : manager.cookieJar()->cookiesForUrl(request.url()))
        cookieList << QString::fromUtf8(cookie.name()) + "=" + shortValue(cookie.value());
    qDebug().noquote() << "     cookies sent:" << cookieList.join("; ");

    for (const auto& header : request.rawHeaderList())
        qDebug().noquote() << "     header:" << header << ":" << request.rawHeader(header);

    if (!body.isEmpty())
        qDebug().noquote() << "     body:" << QString::fromUtf8(body.left(2000));
#endif
}

static void logReply(QNetworkReply* reply, const char* name)
{
#ifndef PEER_NGA_DEBUG
    Q_UNUSED(reply); Q_UNUSED(name);
#else
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    qDebug().noquote() << "PEER <<" << name << status << reason << reply->url().toString();
    if (reply->error() != QNetworkReply::NoError)
        qDebug().noquote() << "     error:" << reply->error() << reply->errorString();

    if (reply->hasRawHeader("Location"))
        qDebug().noquote() << "     Location:" << reply->rawHeader("Location");
    if (reply->hasRawHeader("Content-Type"))
        qDebug().noquote() << "     Content-Type:" << reply->rawHeader("Content-Type");

    // Set-Cookie may contain several cookies separated by newlines
    for (const auto& line : reply->rawHeader("Set-Cookie").split('\n'))
        if (!line.isEmpty())
            qDebug().noquote() << "     Set-Cookie:" << shortValue(line.split(';').first());

    // peek so the processXXX functions can still readAll()
    QByteArray body = reply->peek(reply->bytesAvailable());
    qDebug().noquote() << "     body (" << body.size() << "bytes):"
                       << QString::fromUtf8(body.left(1500)).simplified();
#endif
}

PeerNgaWest2Client::PeerNgaWest2Client(QObject *parent) : QObject(parent),
    getSpectraReply(nullptr), postSpectraReply(nullptr), postSearchReply(nullptr),
    getRecordsReply(nullptr), downloadRecordsReply(nullptr), uploadFileReply(nullptr),
    nRecords(3), isLoggedIn(false), retries(0)
{
    QNetworkCookie cookie("sourceDb_flag", "1");
    // ngawest2.berkeley.edu
    cookie.setDomain("ngawest2.berkeley.edu");
    networkManager.cookieJar()->insertCookie(cookie);
    networkManager.setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);
    
    searchScaleFlag = -1;

    setupConnection();
}

bool PeerNgaWest2Client::loggedIn()
{
    return this->isLoggedIn;
}

void PeerNgaWest2Client::setSession(const QList<QNetworkCookie>& cookies, const QString& csrfToken)
{
    // copy the browser session (_session_id, remember token, ...) into our cookie jar
    for (const auto& cookie : cookies)
        networkManager.cookieJar()->insertCookie(cookie);

    authenticityToken = csrfToken;
    isLoggedIn = !authenticityToken.isEmpty();

#ifdef PEER_NGA_DEBUG
    qDebug().noquote() << "PEER setSession: csrf token length" << csrfToken.length()
                       << "," << cookies.size() << "cookies from browser:";
    for (const auto& cookie : cookies)
        qDebug().noquote() << "     " << cookie.name() << "=" << shortValue(cookie.value())
                           << "domain:" << cookie.domain() << "path:" << cookie.path()
                           << "secure:" << cookie.isSecure() << "httpOnly:" << cookie.isHttpOnly();
    QStringList jar;
    for (const auto& cookie : networkManager.cookieJar()->cookiesForUrl(QUrl(peerHost + "/spectras")))
        jar << QString::fromUtf8(cookie.name());
    qDebug().noquote() << "     cookie jar now sends for /spectras:" << jar.join(", ");
#endif

    if (isLoggedIn)
        emit statusUpdated("Successfully signed in to PEER NGA West 2");
    emit loginFinished(isLoggedIn);
}

void PeerNgaWest2Client::invalidateSession()
{
    isLoggedIn = false;
    authenticityToken.clear();
}

void PeerNgaWest2Client::resubmit()
{
    retries = 0;
    retryPostSpectra();
}

void PeerNgaWest2Client::selectRecords(QList<QPair<double, double>> spectrum, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range, QVariant durationRange, int peerSRkey, int peerSRmeanFlag, int peerFaultType, int peerPulse)
{
    emit selectionStarted();
    emit statusUpdated("Performing Record Selection...");

    this->nRecords = nRecords;
    this->magnitudeRange = magnitudeRange;
    this->distanceRange = distanceRange;
    this->vs30Range = vs30Range;
    this->durationRange = durationRange;
    this->SRkey = peerSRkey;
    this->SRmeanFlag = peerSRmeanFlag;
    this->faultType = peerFaultType;
    this->pulse = peerPulse;

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
    logRequest("POST (multipart)", networkManager, uploadFileRequest,
               "authenticity_token=" + authenticityToken.toUtf8() + "\n"
               + "upload[datafile]=UserSpectrum.csv:\n" + userSpectrum.toUtf8() + "commit=Upload");
    uploadFileReply = networkManager.post(uploadFileRequest, multiPart);
}

void PeerNgaWest2Client::selectRecords(double sds, double sd1, double tl, int nRecords, QVariant magnitudeRange, QVariant distanceRange, QVariant vs30Range, QVariant durationRange, int peerSRkey, int peerSRmeanFlag, int peerFaultType, int peerPulse)
{
    emit selectionStarted();
    emit statusUpdated("Performing Record Selection...");

    this->nRecords = nRecords;
    this->SRkey = peerSRkey;
    this->SRmeanFlag = peerSRmeanFlag;
    this->faultType = peerFaultType;
    this->pulse = peerPulse;

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
    this->durationRange = durationRange;

    recordsToDownload.clear();
    logRequest("POST", networkManager, postSpectraRequest, postSpectraParameters.query().toUtf8());
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
    logRequest("POST", networkManager, postSpectraRequest, postSpectraParameters.query().toUtf8());
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
    const char* name = (reply == uploadFileReply)      ? "uploadFile"
                     : (reply == postSpectraReply)     ? "postSpectra"
                     : (reply == postSearchReply)      ? "postSearch"
                     : (reply == getRecordsReply)      ? "getRecords"
                     : (reply == downloadRecordsReply) ? "downloadRecords"
                                                       : "unknown";
    logReply(reply, name);

    if(reply == uploadFileReply)
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

    reply->deleteLater();
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

void PeerNgaWest2Client::processUploadFileReply()
{
  // qDebug() << "PROCESS UPLOAD FILE REPLY";

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
    logRequest("POST", networkManager, postSpectraRequest, postSpectraParameters.query().toUtf8());
    postSpectraReply = networkManager.post(postSpectraRequest, postSpectraParameters.query().toUtf8());
}

void PeerNgaWest2Client::processPostSpectrumReply()
{
  //    qDebug() << "PROCESS POST SPECTRUIM REPLY";

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

        //if we get redirected to sign in, the session has expired -> need to log in again
        QUrl redirectUrl = postSpectraReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if((statusCode == 302 || statusCode == 303) && redirectUrl.path().endsWith("/members/sign_in")) {
            invalidateSession();
            emit statusUpdated("PEER NGA West 2 session has expired, please log in again");
            emit sessionExpired();
            return;
        }

        retries = 0;
        // Location may be relative or absolute
        auto url = postSpectraReply->url().resolved(redirectUrl);
        auto searchPost = url.toString().remove("/new").remove("/edit").remove("/searches").append("/searches");

        QNetworkRequest searchPostRequest(searchPost);
        searchPostRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrlQuery params;
        params.addQueryItem("authenticity_token", authenticityToken);
        params.addQueryItem("search[DampingRatio]", "0.05");
        params.addQueryItem("search[search_station_name]", "");
        params.addQueryItem("search[search_eq_name]", "");

        // qDebug() << "RECORDS TO DOWNLOAD: " << recordsToDownload;

        if (recordsToDownload.length() == 0) {



            // it is  a search, provide selection quantaties
            params.addQueryItem("search[search_nga_number]", "");
            params.addQueryItem("search[SRkey]", QString::number(this->SRkey));
            params.addQueryItem("search[SRmeanFlag]", QString::number(this->SRmeanFlag));
            params.addQueryItem("search[faultType]", QString::number(this->faultType));
            params.addQueryItem("search[pulse]", QString::number(this->pulse));

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
            if(durationRange.isValid() && !durationRange.isNull())
            {
                auto durationPair = durationRange.value<QPair<double, double>>();
                QString duration = QString::number(durationPair.first) + ',' + QString::number(durationPair.second);
                params.addQueryItem("search[duration]", duration);
            }
        } else {

            QString list;
            int lengthRecords = recordsToDownload.length();
            for (int i=0; i<lengthRecords; i++) {
                list.append(recordsToDownload.at(i));
                if (i != lengthRecords-1)
                    list.append(",");
            }

            // qDebug() << "Peer List: " << list;

            params.addQueryItem("search[search_nga_number]", list);
            params.addQueryItem("search[scale_flag]", "0");
            params.addQueryItem("search[faultType]", "1");
            params.addQueryItem("search[magnitude]", "");
            params.addQueryItem("search[rjb]", "");
            params.addQueryItem("search[rrup]", "");
            params.addQueryItem("search[vs30]", "");
            params.addQueryItem("search[duration]", "");
            params.addQueryItem("search[pulse]", "1");
            //params.addQueryItem("search[Pulse]", "1");
            params.addQueryItem("search[synth_FaultType]", "0");
            params.addQueryItem("search[synth_Adjacency]", "0");
            params.addQueryItem("search[SRkey]", "1");
            params.addQueryItem("search[SRmeanFlag]", "0");
            params.addQueryItem("search[vs30]", "");
            params.addQueryItem("search[rrup]", "");
            params.addQueryItem("search[magnitude]", "");
        }

        logRequest("POST", networkManager, searchPostRequest, params.query().toUtf8());
        postSearchReply = networkManager.post(searchPostRequest, params.query().toUtf8());
    }
}

void PeerNgaWest2Client::processPostSearchReply()
{
  // qDebug() << "PROCESS POST SEARCH REPLY";
    emit statusUpdated("Retrieving Record Selection Results from PEER NGA West 2 Database");

    auto url = postSearchReply->url().resolved(postSearchReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
    auto getRecordsUrl = url.toString().replace("/edit", "/?getRecords=1");


    QNetworkRequest getRecordsRequest(getRecordsUrl);

    //qDebug() << "URL: " << getRecordsRequest.url().toString();    
    
    logRequest("GET", networkManager, getRecordsRequest);
    getRecordsReply = networkManager.get(getRecordsRequest);
}

void PeerNgaWest2Client::processGetRecordsReply()
{

    emit statusUpdated("Downloading Ground Motions from PEER NGA West 2 Database");
    QByteArray replyData = getRecordsReply->readAll();

    // sometimes the search redirects straight to the zip, so this reply IS the records
    // (zip files start with "PK\x03\x04")
    QString contentType = getRecordsReply->header(QNetworkRequest::ContentTypeHeader).toString();
    if (contentType.contains("zip", Qt::CaseInsensitive) || replyData.startsWith("PK\x03\x04")) {
        saveRecords(replyData);
        return;
    }

    auto replyText = QString::fromUtf8(replyData);
    // qDebug() << "::processGetRecordsReply: reply Request:";    
    // qDebug() << "replyText: " << replyText;
#ifdef PEER_NGA_DEBUG
    // the site's notice has the download count/limit and search warnings - pass it on
    static const QRegularExpression noticeRegex(R"(<div id="notice">\s*(.*?)\s*</div>)",
                                                QRegularExpression::DotMatchesEverythingOption);
    auto notice = noticeRegex.match(replyText);
    if (notice.hasMatch())
        qDebug().noquote() << "PEER notice:" << notice.captured(1).simplified();
#endif

    // new site: HTML page with <a href="/tmp/UnscaledAccRecordNNN.zip">
    // old site: JS snippet  window.location.href = "/path/to/records.zip";
    static const QRegularExpression zipLinkRegex(R"(href\s*=\s*["']([^"']+\.zip)["'])",
                                                 QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression jsLinkRegex(R"(window\.location(?:\.href)?\s*=\s*["']([^"']+)["'])");
    auto match = zipLinkRegex.match(replyText);
    if (!match.hasMatch())
        match = jsLinkRegex.match(replyText);
    if (!match.hasMatch()) {
        // save the whole reply so we can see what the site sent back
        QString dumpPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/PeerGetRecordsReply.html";
        QFile dump(dumpPath);
        if (dump.open(QIODevice::WriteOnly)) {
            dump.write(replyText.toUtf8());
            dump.close();
        }
        qDebug().noquote() << "PEER getRecords: no download link found in reply, full reply saved to" << dumpPath;
        emit statusUpdated("Failed to get ground motion download link from PEER NGA West 2 Database");
        emit selectionFinished();
        return;
    }

    // link may be relative or absolute
    QUrl url = getRecordsReply->url().resolved(QUrl(match.captured(1)));
#ifdef PEER_NGA_DEBUG
    qDebug().noquote() << "PEER getRecords: download link" << url.toString();
#endif

    QNetworkRequest downloadRecordsRequest(url);

    // qDebug() << "processGetRecordsReply: URL: " << downloadRecordsRequest.url().toString();        
    
    logRequest("GET", networkManager, downloadRecordsRequest);
    downloadRecordsReply = networkManager.get(downloadRecordsRequest);
}


void PeerNgaWest2Client::processDownloadRecordsReply()
{
    saveRecords(downloadRecordsReply->readAll());
}

void PeerNgaWest2Client::saveRecords(const QByteArray& zipData)
{
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

    file.write(zipData);
    file.close();

    emit statusUpdated("Ground Motions Downloaded Sucessfully");
    emit recordsDownloaded(recordsPath);
}

void PeerNgaWest2Client::retryPostSpectra()
{
    postSpectraParameters.removeQueryItem("authenticity_token");
    postSpectraParameters.addQueryItem("authenticity_token", authenticityToken);
    logRequest("POST", networkManager, postSpectraRequest, postSpectraParameters.query().toUtf8());
    postSpectraReply = networkManager.post(postSpectraRequest, postSpectraParameters.query().toUtf8());
}

void PeerNgaWest2Client::retry()
{
    // network-level failure: re-post (login can no longer be done silently, see sessionExpired)
    if(retries < 5)
    {
        retries++;
        emit statusUpdated("Failed to submit target spectrum to PEER NGA West 2 Database, retrying");
        retryPostSpectra();
    }
    else
    {
        emit statusUpdated("Failed to submit target spectrum to PEER NGA West 2 Database after 5 retries, Please try again shortly.");
        retries = 0;
        emit selectionFinished();
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

