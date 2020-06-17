#include "UserSpectrumWidget.h"
#include <QTableView>
#include <QGridLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QPushButton>

UserSpectrumWidget::UserSpectrumWidget(QWidget* parent): AbstractTargetWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto table = new QTableView(this);
    model = new UserSpectrumModel(this);
    table->setModel(model);
    layout->addWidget(table, 0, 0, 1, 2);
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

