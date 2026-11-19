#ifndef PEERLOGINDIALOG_H
#define PEERLOGINDIALOG_H

#include <QDialog>
#include <QMap>
#include <QNetworkCookie>
#include "PeerNgaWest2Client.h"

class QWebEngineView;
class QWebEngineProfile;
class QLabel;

//
// Shows the PEER NGA West 2 sign-in page in an embedded browser. The site uses
// Cloudflare Turnstile + bot checks, so the user logs in through the real page.
// Once logged in, the session cookies and Rails CSRF token are handed to the
// PeerNgaWest2Client, which then makes its requests with QNetworkAccessManager.
//

class PeerLoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PeerLoginDialog(PeerNgaWest2Client* peerClient, QWidget *parent = nullptr);
    ~PeerLoginDialog();

    // persistent profile shared by all dialogs so "Remember me" survives app restarts
    static QWebEngineProfile* peerProfile();

private:
    void onLoadFinished(bool ok);
    void tryHandoff();
    void tryAutoLogin();

    PeerNgaWest2Client* peerClient;
    QWebEngineView* view;
    QLabel* statusLabel;
    QMap<QByteArray, QNetworkCookie> cookies;  // keyed by name+domain+path
    QString csrfToken;
    bool done;
    bool autoLoginTried;   // only auto-fill once per dialog, so a bad password can't loop
};

#endif // PEERLOGINDIALOG_H
