#include "UserSpectrumModel.h"

UserSpectrumModel::UserSpectrumModel(QObject* parent)
{
    m_spectrum.append({0.01, 0.20});
    m_spectrum.append({0.10, 0.30});
    m_spectrum.append({1.00, 0.40});
    m_spectrum.append({5.00, 0.05});
    m_spectrum.append({10.0, 0.01});
}


int UserSpectrumModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_spectrum.size();
}

int UserSpectrumModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant UserSpectrumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        auto pair = m_spectrum[index.row()];

        if(index.column() == 0)
            return pair.first;
        else if(index.column() == 1)
            return pair.second;
        else
            return QVariant();
    }

    return QVariant();
}


QVariant UserSpectrumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == 0)
            return tr("T [sec.]");
        else
            return tr("SA [g]");

    }
    else if(role == Qt::ToolTipRole && orientation == Qt::Horizontal)
    {
        if (section == 0)
            return tr("Period in seconds");
        else
            return tr("Spectral accelerations in g units");
    }

    return QVariant();
}

QList<QPair<double, double> > UserSpectrumModel::spectrum()
{
    return m_spectrum;
}

void UserSpectrumModel::set(QList<QPair<double, double>> spectrum)
{
    beginResetModel();
    m_spectrum = spectrum;
    endResetModel();
}

void UserSpectrumModel::addPoint(int index)
{
    beginResetModel();
    if (index < 0)
            index = 0;
    m_spectrum.insert(index, m_spectrum[index]);
    endResetModel();
}

void UserSpectrumModel::removePoint(int index)
{
    if (index < 0 || index >= m_spectrum.size())
        return;

    beginResetModel();
    m_spectrum.removeAt(index);
    endResetModel();
}


bool UserSpectrumModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    if (role ==Qt::EditRole)
    {
        bool isDouble = false;
        double doubleValue = value.toString().toDouble(&isDouble);

        if (!isDouble)
            return false;

        auto& pair = m_spectrum[index.row()];

        if(index.column() == 0)
            pair.first = doubleValue;
        else if(index.column() == 1)
            pair.second = doubleValue;

        return true;


    }

    return false;
}

Qt::ItemFlags UserSpectrumModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
