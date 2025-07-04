#ifndef DRMEVENT_H
#define DRMEVENT_H

#include <SimCenterAppWidget.h>
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "drmLocalEventWidget.h"
#include "drmPredefinedEventWidget.h"

class DRMevent : public SimCenterAppWidget {
    Q_OBJECT
public:
    explicit DRMevent(QWidget *parent = nullptr);
    bool outputToJSON(QJsonObject &jsonObject) override;
    bool inputFromJSON(QJsonObject &jsonObject) override;
    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &destDir) override;
    void clear() override;

private:
    QComboBox *eventTypeCombo;
    QStackedWidget *stackedWidget;
    DRMLocalEventWidget *localWidget;
    DRMPredefinedEventWidget *predefWidget;
    QWidget *convWidget;
    QWidget *remoteWidget;
};

#endif // DRMEVENT_H
