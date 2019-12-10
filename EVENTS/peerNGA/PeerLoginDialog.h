#ifndef PEERLOGINDIALOG_H
#define PEERLOGINDIALOG_H

#include <QDialog>
#include "PeerNgaWest2Client.h"
#include <QLineEdit>

class PeerLoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PeerLoginDialog(PeerNgaWest2Client* peerClient, QWidget *parent = nullptr);

signals:

public slots:
private:
    PeerNgaWest2Client* peerClient;
    QLineEdit* usernameEditBox;
    QLineEdit* passwordEditBox;
};

#endif // PEERLOGINDIALOG_H
