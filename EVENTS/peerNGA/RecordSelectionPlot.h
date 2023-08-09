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
    QLineSeries meanSeries;
    QLineSeries plusSigmaSeries;
    QLineSeries minusSigmaSeries;
    QLineSeries targetSeries;
    QChart spectraChart;
    QLogValueAxis xAxis;
    QLogValueAxis yAxis;

    QList<QLineSeries*> currentSelectedSeries;
};

#endif // RECORDSELECTIONPLOT_H
