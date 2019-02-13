#include "SHAMotionWidget.h"
#include <QtWidgets/QVBoxLayout>
#include "RuptureWidget.h"
#include "RecordSelectionWidget.h"
#include "GMPEWidget.h"
#include "IntensityMeasureWidget.h"

SHAMotionWidget::SHAMotionWidget(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
{
    QVBoxLayout* gmToolsLayout = new QVBoxLayout(this);


    RuptureLocation location(37.9,  -122.3);
    m_eqRupture = new PointSourceRupture(7.0, (const RuptureLocation&)location, 0.0, 90.0, this);
    RuptureWidget* ruptureWidget = new RuptureWidget(*this->m_eqRupture, this, Qt::Horizontal);
    gmToolsLayout->addWidget(ruptureWidget, 0);

    m_gmpe = new GMPE(this);
    GMPEWidget* gmpeWidget = new GMPEWidget(*this->m_gmpe, this);
    gmToolsLayout->addWidget(gmpeWidget);

    m_intensityMeasure = new IntensityMeasure(this);
    IntensityMeasureWidget* imWidget = new IntensityMeasureWidget(*m_intensityMeasure, this);
    gmToolsLayout->addWidget(imWidget, 0);

    m_selectionConfig = new RecordSelectionConfig(this);
    RecordSelectionWidget* selectionWidget = new RecordSelectionWidget(*m_selectionConfig, this);
    gmToolsLayout->addWidget(selectionWidget, 0);

    gmToolsLayout->addStretch(1);
    this->setMaximumWidth(450);
}


bool SHAMotionWidget::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    jsonObject["type"] = "HazardBasedEvent";

    QJsonObject groundMotion;
    groundMotion.insert("EqRupture", m_eqRupture->getJson());
    groundMotion.insert("GMPE", m_gmpe->getJson());
    groundMotion.insert("RecordSelection", m_selectionConfig->getJson());
    groundMotion.insert("IntensityMeasure", m_intensityMeasure->getJson());

    jsonObject.insert("GroundMotion", groundMotion);

    return result;
}

bool SHAMotionWidget::inputFromJSON(QJsonObject &rvObject)
{
    bool result = false;

    return result;
}

bool
SHAMotionWidget::outputAppDataToJSON(QJsonObject &jsonObject)
{
    jsonObject["Application"] = "HazardBasedEvent";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    jsonObject["EventClassification"] = "Earthquake";

    return true;
}


bool
SHAMotionWidget::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    return true;
}
