#include "UserSpectrumWidget.h"
#include <QTableView>
#include <QGridLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>

UserSpectrumWidget::UserSpectrumWidget(QWidget* parent): AbstractTargetWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto table = new QTableView(this);
    model = new UserSpectrumModel(this);
    table->setModel(model);
    layout->addWidget(table, 0, 0, 1, 3);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto addButton = new QPushButton("Add");
    layout->addWidget(addButton, 1, 0);
    connect(addButton, &QPushButton::clicked, this, [this, table](){
        model->addPoint(table->currentIndex().row());
    });

    auto removeButton = new QPushButton("Remove");
    layout->addWidget(removeButton, 1, 1);
    connect(removeButton, &QPushButton::clicked, this, [this, table](){
        model->removePoint(table->currentIndex().row());
    });

    auto loadButton = new QPushButton("Load CSV");
    layout->addWidget(loadButton, 1, 2);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadCSV()));

}

QJsonObject UserSpectrumWidget::serialize() const
{
    QJsonObject json;
    QJsonArray spectrumArray;
    for (auto& pair: model->spectrum())
    {
        QJsonArray point;
        point.append(pair.first);
        point.append(pair.second);
        spectrumArray.append(point);
    }
    json["UserSpectrum"] = spectrumArray;

    return json;
}

void UserSpectrumWidget::deserialize(const QJsonObject &json)
{
    QList<QPair<double, double>> spectrum;
    for (auto point: json["UserSpectrum"].toArray())
        spectrum.append({point.toArray()[0].toDouble(), point.toArray()[1].toDouble()});

    model->set(spectrum);
}

QList<QPair<double, double>> UserSpectrumWidget::spectrum() const
{
    return model->spectrum();
}

void UserSpectrumWidget::loadCSV(void)
{
    // loading a csv table of user-specified spectrum
    spectrumfilepath=QFileDialog::getOpenFileName(this,tr("Spectrum CSV"));
    if(spectrumfilepath.isEmpty())
    {
        spectrumfilepath = "NULL";
        qDebug()<<"Path is empty.";
        return;
    }
    this->loadSpectrum();

}

void UserSpectrumWidget::loadSpectrum(void)
{
    qDebug()<<"Loading spectrum file...";
    QFile tmpFile(spectrumfilepath);
    if (!tmpFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Cannot find the file: " + spectrumfilepath + "\nCheck your directory and try again.";
        return;
    }
    QStringList rowLines;
    while (!tmpFile.atEnd())
    {
        QString line = tmpFile.readLine();
        rowLines << line;
    }
    auto numRows = rowLines.size();
    if(numRows == 0)
    {
        qDebug()<<"Error in parsing the .csv file "<<tmpFile <<" in UserDefinedDatabase::parseCSV file";
        return;
    }
    int row_num = 0;
    QList<QPair<double,double>> tmpSpectrum;
    QString separator = ",";
    QStringList tmpData;
    for(auto&& it: rowLines)
    {
        if (row_num > 0)
        {
            tmpData = it.split(separator);
            if (tmpData.size() == 1)
            {
                // try spacings
                separator = "\\s+";
                tmpData = it.split(QRegExp(separator));
            }
            auto tmpT = tmpData.at(0).toDouble();
            auto tmpSa = tmpData.at(1).toDouble();
            // validating
            if (tmpT < 0.0 || tmpSa < 0.0)
            {
                qDebug()<<"Negative values in the spectrum.";
                break;
            }
            tmpSpectrum.append({tmpT, tmpSa});
        }
        row_num = row_num+1;
    }
    // setting the spectrum
    model->set(tmpSpectrum);

}

