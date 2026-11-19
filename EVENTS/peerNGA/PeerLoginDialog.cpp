#include "PeerLoginDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineCookieStore>
#include <QJsonArray>
#include <QJsonDocument>

//
// Optional built-in PEER account. PeerMasterLogin.h is NOT in git (see .gitignore and
// PeerMasterLogin.h.example). If it is present at build time the sign-in form is filled in
// and submitted automatically; otherwise the user logs in through the page as normal.
//
#if __has_include("PeerMasterLogin.h")
#include "PeerMasterLogin.h"
#define PEER_HAVE_MASTER_LOGIN 1
#endif

static const char* peerHostName = "ngawest2.berkeley.edu";
static const char* peerSignInUrl = "https://ngawest2.berkeley.edu/members/sign_in";

static bool isPeerCookie(const QNetworkCookie& cookie)
{
    QString domain = cookie.domain();
    if (domain.startsWith('.'))
        domain.remove(0, 1);
    return QString(peerHostName).endsWith(domain) && domain.endsWith("berkeley.edu");
}

static QByteArray cookieKey(const QNetworkCookie& cookie)
{
    return cookie.name() + '|' + cookie.domain().toUtf8() + '|' + cookie.path().toUtf8();
}

QWebEngineProfile* PeerLoginDialog::peerProfile()
{
    static QWebEngineProfile* profile = nullptr;
    if (profile == nullptr) {
        // named profile => cookies stored on disk (persistent)
        profile = new QWebEngineProfile("PeerNgaWest2", QCoreApplication::instance());
        profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    }
    return profile;
}

PeerLoginDialog::PeerLoginDialog(PeerNgaWest2Client* peerClient, QWidget *parent) : QDialog(parent),
    peerClient(peerClient), done(false), autoLoginTried(false)
{
    this->setWindowTitle("Log in to PEER NGA West 2 Ground Motion Database");
    this->resize(900, 750);

    auto layout = new QVBoxLayout(this);

    statusLabel = new QLabel("Please log in with your PEER credentials (or sign up for a new account) "
                             "using the page below. This window will close once you are logged in.");
    statusLabel->setWordWrap(true);
    layout->addWidget(statusLabel);

    auto profile = peerProfile();

    // track cookies as the browser sets them (includes httponly session cookie)
    auto store = profile->cookieStore();
    connect(store, &QWebEngineCookieStore::cookieAdded, this, [this](const QNetworkCookie& cookie) {
        if (!isPeerCookie(cookie))
            return;
        cookies.insert(cookieKey(cookie), cookie);
        if (!csrfToken.isEmpty())
            tryHandoff();
    });
    connect(store, &QWebEngineCookieStore::cookieRemoved, this, [this](const QNetworkCookie& cookie) {
        cookies.remove(cookieKey(cookie));
    });
    store->loadAllCookies();

    view = new QWebEngineView(this);
    view->setPage(new QWebEnginePage(profile, view));
    layout->addWidget(view, 1);

    connect(view, &QWebEngineView::loadFinished, this, &PeerLoginDialog::onLoadFinished);

    view->load(QUrl(peerSignInUrl));
}

PeerLoginDialog::~PeerLoginDialog()
{
    // page must be gone before profile; profile is app-lifetime so this is just tidy
    delete view;
}

void PeerLoginDialog::onLoadFinished(bool ok)
{
    if (!ok || done)
        return;

    QUrl url = view->url();
    if (url.host() != peerHostName)
        return;

    // on the sign-in page: fill in the built-in account if this build has one
    if (url.path() == "/members/sign_in") {
        tryAutoLogin();
        return;
    }

    // still on sign_up, password reset, ... => not logged in yet
    if (url.path().startsWith("/members"))
        return;

    // Devise redirected us off the login page => logged in. Grab the session CSRF token
    // (Devise rotates it on login, so the one on the sign-in page is no longer valid).
    view->page()->runJavaScript(
        "(function(){var m=document.querySelector('meta[name=\"csrf-token\"]');return m?m.content:'';})()",
        [this](const QVariant& result) {
            csrfToken = result.toString();
            if (csrfToken.isEmpty()) {
                statusLabel->setText("Logged in, but could not obtain a session token from the PEER site.");
                return;
            }
            tryHandoff();
        });
}

void PeerLoginDialog::tryAutoLogin()
{
#ifdef PEER_HAVE_MASTER_LOGIN
    if (autoLoginTried) {
        // we came back to sign_in after submitting => login was rejected
        statusLabel->setText("Automatic login with the built-in PEER account failed. "
                             "Please log in with your own PEER credentials below.");
        return;
    }
    autoLoginTried = true;
    statusLabel->setText("Logging in to PEER automatically, please wait ... "
                         "(if a Cloudflare check box appears, please tick it)");

    // pass credentials as a JSON array so quotes etc. are escaped safely for JS
    QJsonArray credentials{QString(PEER_MASTER_EMAIL), QString(PEER_MASTER_PASSWORD)};
    QString args = QString::fromUtf8(QJsonDocument(credentials).toJson(QJsonDocument::Compact));

    // Fill the form, then submit once Cloudflare Turnstile has produced its token.
    // The site also uses invisible_captcha, which rejects forms submitted within a few
    // seconds of page load, so wait at least 5s. Leave the honeypot (member[subtitle]) empty.
    QString script = QString(R"JS(
(function(a) {
  var f = document.getElementById('new_member');
  if (!f) return 'no form';
  f.querySelector('#member_email').value = a[0];
  f.querySelector('#member_password').value = a[1];
  var remember = f.querySelector('#member_remember_me');
  if (remember) remember.checked = true;
  var start = Date.now();
  var timer = setInterval(function() {
    var t = f.querySelector('[name="cf-turnstile-response"]');
    var elapsed = Date.now() - start;
    if (t && t.value.length > 0 && elapsed > 5000) {
      clearInterval(timer);
      var button = f.querySelector('[type="submit"]');
      if (f.requestSubmit) f.requestSubmit(button); else f.submit();
    } else if (elapsed > 120000) {
      clearInterval(timer);
    }
  }, 500);
  return 'filled';
})(%1)
)JS").arg(args);

    view->page()->runJavaScript(script);
#endif
}

void PeerLoginDialog::tryHandoff()
{
    if (done)
        return;

    bool haveSession = false;
    for (const auto& cookie : cookies)
        if (cookie.name() == "_session_id")
            haveSession = true;

    if (!haveSession)
        return; // wait for cookieAdded

    done = true;
    peerClient->setSession(cookies.values(), csrfToken);
    this->accept();
}
