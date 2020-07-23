#ifndef USERSPECTRUMMODEL_H
#define USERSPECTRUMMODEL_H
#include <QAbstractTableModel>

class UserSpectrumModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UserSpectrumModel(QObject* parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<QPair<double, double>> m_spectrum;

public:
    QList<QPair<double, double>> spectrum();
    void set(QList<QPair<double, double>> spectrum);
    void addPoint(int index);
    void removePoint(int index);


};

#endif // USERSPECTRUMMODEL_H
