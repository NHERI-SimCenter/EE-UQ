#ifndef UNIFORMMOTIONINPUT_H
#define UNIFORMMOTIONINPUT_H

#include <SimCenterAppWidget.h>
class QLineEdit;
class QComboBox;
class QPushButton;
class EarthquakeRecord;

class RandomVariableInputWidget;

class UniformMotionInput : public SimCenterAppWidget
{
    Q_OBJECT
public:

    explicit UniformMotionInput(RandomVariableInputWidget *, SimCenterWidget *parent = nullptr);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputToJSON(QJsonObject &rvObject);
    QString getApplicationName(void);

    QLineEdit *scaleFactor;
    QComboBox *eqMotion;
    QPushButton *addMotion;

    SimCenterWidget *theUniformInputMotion;
    std::map <QString, EarthquakeRecord *> records;
    EarthquakeRecord *theCurrentRecord;

signals:

public slots:

private:
    RandomVariableInputWidget *theRandomVariableInputWidget;

};

#endif // UNIFORMMOTIONINPUT_H
