// Written: fmckenna

/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include "InputWidgetEarthquakeEvent.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStackedWidget>
#include <QComboBox>


#include <QPushButton>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <sectiontitle.h>
//#include <InputWidgetEDP.h>

#include <UniformMotionInput.h>
#include "SHAMotionWidget.h"

InputWidgetEarthquakeEvent::InputWidgetEarthquakeEvent(QWidget *parent)
    : SimCenterWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    //
    // the selection part
    //

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setText(QString("Loading Type"));
    eventSelection = new QComboBox();
    eventSelection->addItem(tr("Uniform"));
    eventSelection->addItem(tr("SHA Based Event"));
    eventSelection->setItemData(1, "A Seismic event using Seismic Hazard Analysis and Record Selection/Scaling", Qt::ToolTipRole);

    theSelectionLayout->addWidget(label);
    theSelectionLayout->addWidget(eventSelection);
    theSelectionLayout->addStretch();
    layout ->addLayout(theSelectionLayout);

    //
    // create the stacked widget
    //
    theStackedWidget = new QStackedWidget();

    //
    // create the individual load widgets & add to stacked widget
    //

    theUniformInputMotion = new UniformMotionInput();
    theStackedWidget->addWidget(theUniformInputMotion);

    //Adding SHA based ground motion widget
    m_SHAMotionWidget = new SHAMotionWidget(this);
    theStackedWidget->addWidget(m_SHAMotionWidget);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);

    connect(eventSelection,SIGNAL(currentIndexChanged(QString)),this,SLOT(eventSelectionChanged(QString)));
}

InputWidgetEarthquakeEvent::~InputWidgetEarthquakeEvent()
{

}


bool
InputWidgetEarthquakeEvent::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    if(eventSelection->currentIndex() == 1)
    {
        //SHA based event is selected
        return m_SHAMotionWidget->outputToJSON(jsonObject);
    }

    return result;
}


bool
InputWidgetEarthquakeEvent::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;

    return result;
}

void InputWidgetEarthquakeEvent::eventSelectionChanged(const QString &arg1)
{
    // if more data than just num samples and seed code would go here to add or remove widgets from layout
    if (arg1 == "Uniform")
        theStackedWidget->setCurrentIndex(0);
    else if(arg1 == "SHA Based Event")
        theStackedWidget->setCurrentIndex(1);
}

