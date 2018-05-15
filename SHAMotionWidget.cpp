#include "SHAMotionWidget.h"
#include <QtWidgets/QVBoxLayout>
#include "RuptureWidget.h"
#include "RecordSelectionWidget.h"
#include "GMPEWidget.h"

SHAMotionWidget::SHAMotionWidget(QWidget *parent) : SimCenterWidget(parent)
{
    QVBoxLayout* gmToolsLayout = new QVBoxLayout(this);


    RuptureLocation location(37.9,  -122.3);
    m_eqRupture = new PointSourceRupture(7.0, (const RuptureLocation&)location, 0.0, 90.0, this);
    RuptureWidget* ruptureWidget = new RuptureWidget(*this->m_eqRupture, this, Qt::Horizontal);
    gmToolsLayout->addWidget(ruptureWidget, 0);

    m_gmpe = new GMPE();
    GMPEWidget* gmpeWidget = new GMPEWidget(*this->m_gmpe, this);
    gmToolsLayout->addWidget(gmpeWidget);

    m_selectionConfig = new RecordSelectionConfig();
    RecordSelectionWidget* selectionWidget = new RecordSelectionWidget(*m_selectionConfig, this);
    gmToolsLayout->addWidget(selectionWidget, 0);
    gmToolsLayout->addStretch(1);
    this->setMaximumWidth(450);
}


bool SHAMotionWidget::outputToJSON(QJsonObject &rvObject)
{
    bool result = false;

    return result;
}

bool SHAMotionWidget::inputFromJSON(QJsonObject &rvObject)
{
    bool result = false;

    return result;
}
