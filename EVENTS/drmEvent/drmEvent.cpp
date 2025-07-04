#include "drmEvent.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>

DRMevent::DRMevent(QWidget *parent) : SimCenterAppWidget(parent) {
    eventTypeCombo = new QComboBox(this);
    eventTypeCombo->addItem("Local");
    eventTypeCombo->addItem("Predefined-DesignSafe");
    eventTypeCombo->addItem("Convolution");
    eventTypeCombo->addItem("Remote-DesignSafe");

    stackedWidget = new QStackedWidget(this);
    localWidget = new DRMLocalEventWidget(this);
    predefWidget = new DRMPredefinedEventWidget(this);
    convWidget = new QLabel("Convolution event selection not implemented yet.", this);
    remoteWidget = new QLabel("Remote-DesignSafe event selection not implemented yet.", this);

    stackedWidget->addWidget(localWidget);
    stackedWidget->addWidget(predefWidget);
    stackedWidget->addWidget(convWidget);
    stackedWidget->addWidget(remoteWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("DRM Event Type:"));
    mainLayout->addWidget(eventTypeCombo);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    connect(eventTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            stackedWidget, &QStackedWidget::setCurrentIndex);
}

bool DRMevent::outputToJSON(QJsonObject &jsonObject) {
    // Only output the currently selected event
    int idx = eventTypeCombo->currentIndex();
    if (idx == 0) {
        return localWidget->outputToJSON(jsonObject);
    } else if (idx == 1) {
        return predefWidget->outputToJSON(jsonObject);
    }
    return true;
}

bool DRMevent::inputFromJSON(QJsonObject &jsonObject) {
    QStringList keys = jsonObject.keys();
    for (const QString &key : keys) {
    }
    if (jsonObject.contains("type") && jsonObject["type"].toString() == "DRM") {
        if (jsonObject.contains("system") && jsonObject["system"].toString() == "local") {
            eventTypeCombo->setCurrentIndex(0);
            return localWidget->inputFromJSON(jsonObject);
        } else if (jsonObject.contains("system") && jsonObject["system"].toString() == "predefined-designsafe") {
            eventTypeCombo->setCurrentIndex(1);
            return predefWidget->inputFromJSON(jsonObject);
        }
    }
    // print key names for debugging
    return true;
}

bool DRMevent::outputAppDataToJSON(QJsonObject &jsonObject) {
    int idx = eventTypeCombo->currentIndex();
    if (idx == 0) {
        return localWidget->outputAppDataToJSON(jsonObject);
    } else if (idx == 1) {
        return predefWidget->outputAppDataToJSON(jsonObject);
    }
    return true;
}

bool DRMevent::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}

bool DRMevent::copyFiles(QString &destDir) {
    // Implement as needed
    return true;
}

void DRMevent::clear() {
    localWidget->clear();
    predefWidget->clear();
    eventTypeCombo->setCurrentIndex(0);
}
