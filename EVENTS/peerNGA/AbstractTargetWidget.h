#ifndef ABSTRACTTARGETWIDGET_H
#define ABSTRACTTARGETWIDGET_H

#include <QWidget>
#include "AbstractJsonSerializable.h"

class AbstractTargetWidget : public QWidget, public AbstractJsonSerializable
{
    Q_OBJECT

public:
    explicit AbstractTargetWidget(QWidget *parent = nullptr):QWidget(parent){};

    virtual ~AbstractTargetWidget(){};

    virtual QList<QPair<double, double>> spectrum() const = 0;

signals:
    virtual void targetChanged();
    void statusUpdated(QString status);

};

#endif // ABSTRACTTARGETWIDGET_H
