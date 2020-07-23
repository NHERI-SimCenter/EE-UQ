#include "RecordSelectionPlot.h"
#include <QVBoxLayout>
#include <QChartView>
#include <QPushButton>
#include <QtCharts/QLegendMarker>

RecordSelectionPlot::RecordSelectionPlot(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    spectraChart.setTitle("Response Spectra");
    spectraChart.addSeries(&meanSeries);
    spectraChart.addSeries(&plusSigmaSeries);
    spectraChart.addSeries(&minusSigmaSeries);
    spectraChart.addSeries(&targetSeries);

    QPen meanPen;
    meanPen.setColor(Qt::blue);
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

    auto chartView = new QtCharts::QChartView(&spectraChart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumWidth(500);
    chartView->setMinimumHeight(500);

    layout->addWidget(chartView);

    this->setMinimumWidth(200);
    this->setMinimumHeight(200);
}

void RecordSelectionPlot::setMean(QVector<double> periods, QVector<double> sa)
{
    meanSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
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
    plusSigmaSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
        plusSigmaSeries << QPointF(periods[i], sa[i]);
    }
}

void RecordSelectionPlot::setMeanMinusSigma(QVector<double> periods, QVector<double> sa)
{
    minusSigmaSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
        minusSigmaSeries << QPointF(periods[i], sa[i]);
    }
}

void RecordSelectionPlot::setTargetSpectrum(QVector<double> periods, QVector<double> sa)
{
    targetSeries.clear();

    for(int i = 0; i < periods.size(); i++)
    {
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
        auto series = new QtCharts::QLineSeries(this);
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
