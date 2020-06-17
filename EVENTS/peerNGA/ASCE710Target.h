#ifndef ASCE710TARGET_H
#define ASCE710TARGET_H

#include <QWidget>
#include <QLineEdit>
#include "AbstractTargetWidget.h"

class ASCE710Target : public AbstractTargetWidget
{
    Q_OBJECT
public:
    explicit ASCE710Target(QWidget *parent = nullptr);

signals:

private:
    QLineEdit* sdsEditBox;
    QLineEdit* sd1EditBox;
    QLineEdit* tlEditBox;

public:
    QJsonObject serialize() const override;
    void deserialize(const QJsonObject &json) override;


    // AbstractTargetWidget interface
public:
    QList<QPair<double, double> > spectrum() const override;
    double sds();
    double sd1();
    double tl();

};

#endif // ASCE710TARGET_H
