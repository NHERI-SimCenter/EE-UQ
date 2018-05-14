#ifndef UNIFORMMOTIONINPUT_H
#define UNIFORMMOTIONINPUT_H

#include <SimCenterWidget.h>
class QLineEdit;
class QComboBox;
class QPushButton;

class UniformMotionInput : public SimCenterWidget
{
    Q_OBJECT
public:

    explicit UniformMotionInput(QWidget *parent = nullptr);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputToJSON(QJsonObject &rvObject);

    QLineEdit *scaleFactor;
    QComboBox *eqMotion;
    QPushButton *addMotion;

    SimCenterWidget *theUniformInputMotion;

signals:

public slots:

private:

};

#endif // UNIFORMMOTIONINPUT_H
