#include "RecordSelectionPlot.h"
#include <QVBoxLayout>
#include <QChartView>
#include <QPushButton>
#include <QtCharts/QLegendMarker>

RecordSelectionPlot::RecordSelectionPlot(QWidget *parent) :
  QWidget(parent),
  addMean(false),addPlus(false),addMinus(false), addTarget(false)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    spectraChart.setTitle("Response Spectra");

    QPen meanPen;
    meanPen.setColor(Qt::black);
    meanPen.setWidth(2);
    meanSeries.setPen(meanPen);

    QPen redPen;
    redPen.setColor(Qt::darkRed);
    redPen.setWidth(2);
    redPen.setStyle(Qt::DotLine);
    minusSigmaSeries.setPen(redPen);
    plusSigmaSeries.setPen(redPen);

    QPen bluePen;
    bluePen.setColor(Qt::blue);
    bluePen.setWidth(2);
    targetSeries.setPen(bluePen);

    //Setting names
    meanSeries.setName("Mean");
    minusSigmaSeries.setName("Mean - Std. Dev.");
    plusSigmaSeries.setName("Mean + Std. Dev.");
    targetSeries.setName("Target");

    //Setting x Axis
    xAxis.setTitleText("Period [sec.]");
    spectraChart.addAxis(&xAxis, Qt::AlignBottom);
    xAxis.setLabelFormat("%1.2f");
    xAxis.setMinorTickCount(-1);

    //Attaching x Axis
    meanSeries.attachAxis(&xAxis);
    minusSigmaSeries.attachAxis(&xAxis);
    plusSigmaSeries.attachAxis(&xAxis);
    targetSeries.attachAxis(&xAxis);

    //Setting y Axis
    yAxis.setTitleText("Spectral Acceleration [g.]");
    spectraChart.addAxis(&yAxis, Qt::AlignLeft);
    yAxis.setLabelFormat("%1.2f");
    yAxis.setMinorTickCount(-1);
    meanSeries.attachAxis(&yAxis);
    minusSigmaSeries.attachAxis(&yAxis);
    plusSigmaSeries.attachAxis(&yAxis);
    targetSeries.attachAxis(&yAxis);

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    auto chartView = new QChartView(&spectraChart, this);
#else
    auto chartView = new QtCharts::QChartView(&spectraChart, this);
#endif

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumWidth(500);
    chartView->setMinimumHeight(500);

    layout->addWidget(chartView);

    this->setMinimumWidth(200);
    this->setMinimumHeight(200);
}


RecordSelectionPlot::~RecordSelectionPlot() {
    meanSeries.clear();
    plusSigmaSeries.clear();
    minusSigmaSeries.clear();
    targetSeries.clear();
    spectraChart.removeSeries(&meanSeries);
    spectraChart.removeSeries(&plusSigmaSeries);
    spectraChart.removeSeries(&minusSigmaSeries);
    spectraChart.removeSeries(&targetSeries);                
}
  
void RecordSelectionPlot::setMean(QVector<double> periods, QVector<double> sa)
{
  if (addMean == false) {
    addMean = true;
    spectraChart.addSeries(&meanSeries);
    meanSeries.attachAxis(&xAxis);
    meanSeries.attachAxis(&yAxis);    
  }

    meanSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
        if (periods[i] > 0 && sa[i] > 0.0)
            meanSeries << QPointF(periods[i], sa[i]);
    }

    xAxis.setMin(periods[0]);
    xAxis.setMax(periods[periods.size() - 1]);

    yAxis.setMin(0.01);
    yAxis.setMax(10);

    xAxis.setMinorTickCount(-1);
    yAxis.setMinorTickCount(-1);
}

void RecordSelectionPlot::setMeanPlusSigma(QVector<double> periods, QVector<double> sa)
{
  if (addPlus == false) {
    addPlus = true;
    spectraChart.addSeries(&plusSigmaSeries);
    plusSigmaSeries.attachAxis(&xAxis);
    plusSigmaSeries.attachAxis(&yAxis);        
  }    
  plusSigmaSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
        if (periods[i] > 0 && sa[i] > 0.0)
            plusSigmaSeries << QPointF(periods[i], sa[i]);
    }
}

void RecordSelectionPlot::setMeanMinusSigma(QVector<double> periods, QVector<double> sa)
{
  if (addMinus == false) {
    addMinus = true;
    spectraChart.addSeries(&minusSigmaSeries);
    minusSigmaSeries.attachAxis(&xAxis);
    minusSigmaSeries.attachAxis(&yAxis);        
  }  
    minusSigmaSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
        if (periods[i] > 0 && sa[i] > 0.0)
            minusSigmaSeries << QPointF(periods[i], sa[i]);
    }
}

void RecordSelectionPlot::setTargetSpectrum(QVector<double> periods, QVector<double> sa)
{

  if (addTarget == false) {
    addTarget = true;
    spectraChart.addSeries(&targetSeries);
    targetSeries.attachAxis(&xAxis);
    targetSeries.attachAxis(&yAxis);        
  }    
    targetSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
        if (periods[i] > 0 && sa[i] > 0.0)
            targetSeries << QPointF(periods[i], sa[i]);
    }
}

void RecordSelectionPlot::setSelectedSpectra(QVector<double> periods, QVector<QVector<double> > spectra)
{
    for(auto oldSeries: currentSelectedSeries)
    {
        spectraChart.removeSeries(oldSeries);
    }

    currentSelectedSeries.clear();

    bool firstLegendShown = false;
    for(auto spectrum: spectra)
    {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        auto series = new QLineSeries(this);
#else
        auto series = new QtCharts::QLineSeries(this);
#endif
        spectraChart.addSeries(series);
        currentSelectedSeries.push_back(series);

        for(int i = 0; i < periods.size(); i++)
        {
            (*series) << QPointF(periods[i], spectrum[i]);
        }

        series->attachAxis(&xAxis);
        series->attachAxis(&yAxis);
        series->setOpacity(0.5);
        QPen selectionPen;
        selectionPen.setColor(Qt::gray);
        selectionPen.setWidth(2);
        series->setPen(selectionPen);

        if(firstLegendShown)
        {
            auto marker = spectraChart.legend()->markers(series)[0];
            marker->setVisible(false);
        }
        else
        {
            firstLegendShown = true;
            series->setName("Selected");
        }
    }
}

void RecordSelectionPlot::highlightSpectra(QList<int> indices)
{
    QPen grayPen;
    grayPen.setColor(Qt::gray);
    grayPen.setWidth(2);
    for(auto series:currentSelectedSeries)
        series->setPen(grayPen);


    for (auto index: indices)
    {
        if(index >= currentSelectedSeries.size())
            continue;

        auto series = currentSelectedSeries[index];
        auto pen = series->pen();
        pen.setColor(Qt::green);
        series->setPen(pen);
    }
}
