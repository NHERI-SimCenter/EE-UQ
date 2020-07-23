#ifndef RECORDSELECTIONPLOT_H
#define RECORDSELECTIONPLOT_H

#include <QWidget>
#include <QLineSeries>
#include <QLogValueAxis>
#include <QChart>

class RecordSelectionPlot : public QWidget
{
    Q_OBJECT
public:
    explicit RecordSelectionPlot(QWidget *parent = nullptr);
    void setMean(QVector<double> periods, QVector<double> sa);
    void setMeanPlusSigma(QVector<double> periods, QVector<double> sa);
    void setMeanMinusSigma(QVector<double> periods, QVector<double> sa);
    void setTargetSpectrum(QVector<double> periods, QVector<double> sa);
    void setSelectedSpectra(QVector<double> periods, QVector<QVector<double>> spectra);
    void highlightSpectra(QList<int> indices);

signals:

public slots:

private:
    QtCharts::QLineSeries meanSeries;
    QtCharts::QLineSeries plusSigmaSeries;
    QtCharts::QLineSeries minusSigmaSeries;
    QtCharts::QLineSeries targetSeries;
    QtCharts::QChart spectraChart;
    QtCharts::QLogValueAxis xAxis;
    QtCharts::QLogValueAxis yAxis;

    QList<QtCharts::QLineSeries*> currentSelectedSeries;
};

#endif // RECORDSELECTIONPLOT_H
