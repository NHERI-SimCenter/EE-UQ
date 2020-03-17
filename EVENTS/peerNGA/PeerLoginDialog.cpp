#include "PeerLoginDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QCheckBox>

PeerLoginDialog::PeerLoginDialog(PeerNgaWest2Client* peerClient, QWidget *parent) : QDialog(parent),
    peerClient(peerClient)
{
    this->setWindowTitle("Log in to PEER NGA West 2 Ground Motion Database");
    auto layout = new QGridLayout(this);

    layout->addWidget(new QLabel("Please use your PEER credentials to login to NGA West 2 Database"), 0, 0, 1, 2);
    auto signUpLabel =
            new QLabel("If you dont have a PEER account, you can <a href=\"https://ngawest2.berkeley.edu/users/sign_up\">sign up for a new account</a>");
    signUpLabel->setTextFormat(Qt::RichText);
    signUpLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    signUpLabel->setOpenExternalLinks(true);
    layout->addWidget(signUpLabel, 1, 0, 1, 2);

    layout->addWidget(new QLabel("Username"), 2, 0);
    usernameEditBox = new QLineEdit();
    layout->addWidget(usernameEditBox, 2, 1);
    QSettings settings;
    auto peerUsernameSetting = settings.value("PeerUsername");
    if(peerUsernameSetting.isValid() && !peerUsernameSetting.isNull())
        usernameEditBox->setText(peerUsernameSetting.toString());

    //Password
    layout->addWidget(new QLabel("Password"), 3, 0);
    passwordEditBox = new QLineEdit();
    passwordEditBox->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordEditBox, 3, 1);

    auto peerPasswordSetting = settings.value("PeerPassword");
    if(peerPasswordSetting.isValid() && !peerPasswordSetting.isNull())
        passwordEditBox->setText(peerPasswordSetting.toString());

    //Save Password
    auto savePassword = new QCheckBox("save password");
    layout->addWidget(savePassword, 4,1);

    if(usernameEditBox->text().isEmpty())
        usernameEditBox->setFocus();
    else
        passwordEditBox->setFocus();

    auto loginButton = new QPushButton("Log In");
    layout->addWidget(loginButton, 5, 0, 1, 2);

    connect(loginButton, &QPushButton::clicked, this, [this, loginButton, savePassword](){
        loginButton->setEnabled(false);
        loginButton->setDown(true);
        QSettings settings;
        settings.setValue("PeerUsername", usernameEditBox->text());
        if(savePassword->checkState() == Qt::Checked)
            settings.setValue("PeerPassword", passwordEditBox->text());
        this->peerClient->signIn(usernameEditBox->text(), passwordEditBox->text());
    });

    connect(this->peerClient, &PeerNgaWest2Client::loginFinished, this, [this, loginButton](bool result){
        if (result)
            this->accept();
        else
        {
            loginButton->setEnabled(true);
            loginButton->setDown(false);
        }
    });
}
