#ifndef SHAMOTIONWIDGET_H
#define SHAMOTIONWIDGET_H

#include <QWidget>
#include "SimCenterWidget.h"
#include "PointSourceRupture.h"
#include "GMPE.h"
#include "RecordSelectionConfig.h"

class SHAMotionWidget : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit SHAMotionWidget(QWidget *parent = nullptr);

signals:

public slots:

    // SimCenterWidget interface
public:
    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

private:
    PointSourceRupture* m_eqRupture;
    GMPE* m_gmpe;
    RecordSelectionConfig* m_selectionConfig;
};

#endif // SHAMOTIONWIDGET_H
