#ifndef SHAMOTIONWIDGET_H
#define SHAMOTIONWIDGET_H

#include "SimCenterAppWidget.h"
#include "PointSourceRupture.h"
#include "GMPE.h"
#include "RecordSelectionConfig.h"

class RandomVariableInputWidget;

class SHAMotionWidget : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit SHAMotionWidget(RandomVariableInputWidget *, QWidget *parent = nullptr);

signals:

public slots:

    // SimCenterWidget interface
public:
    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &rvObject);
    bool inputAppDataFromJSON(QJsonObject &rvObject);
private:
    PointSourceRupture* m_eqRupture;
    GMPE* m_gmpe;
    RecordSelectionConfig* m_selectionConfig;

    RandomVariableInputWidget *theRandomVariableInputWidget;
};

#endif // SHAMOTIONWIDGET_H
