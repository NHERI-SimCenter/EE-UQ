#include "UniformMotionInput.h"
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


UniformMotionInput::UniformMotionInput(QWidget *parent) : SimCenterWidget(parent)
{
    QGridLayout *inputMotionLayout = new QGridLayout();
    QLabel *entryLabel = new QLabel();
    entryLabel->setText(tr("Input Motion"));

    eqMotion = new QComboBox();

    QLabel *scaleLabel = new QLabel(tr("Scale Factor"));
    scaleFactor = new QLineEdit();
    scaleFactor->setMaximumWidth(100);

    inputMotionLayout->addWidget(entryLabel,0,0);
    inputMotionLayout->addWidget(eqMotion,0,1);
    inputMotionLayout->addWidget(scaleLabel,0,3);
    inputMotionLayout->addWidget(scaleFactor,0,4);
    inputMotionLayout->setRowStretch(2,1);
    inputMotionLayout->setColumnStretch(5,1);
    // inputMotionLayout->addStretch();

    addMotion = new QPushButton("Add");
    inputMotionLayout->addWidget(addMotion,1,4);

    this->setLayout(inputMotionLayout);
}

bool
UniformMotionInput::inputFromJSON(QJsonObject &rvObject)
{
        return true;
}

bool
UniformMotionInput::outputToJSON(QJsonObject &rvObject)
{
        return true;
}
