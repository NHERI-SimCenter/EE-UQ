#ifndef SIMCENTERINTENSITYMEASURECOMBO_H
#define SIMCENTERINTENSITYMEASURECOMBO_H
/* *****************************************************************************
Copyright (c) 2016-2022, The Regents of the University of California (Regents).
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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Written by: Kuanshi Zhong, Stevan Gavrilovic, Frank McKenna

//#include "JsonSerializable.h"

#include <QComboBox>

namespace SimCenterEQ {

class IntensityMeasure :  QObject{

    Q_OBJECT

public:
    enum Type {
        UNDEFINED = 0x000,
        PEAK = 0x100,
        PGA, PGV, PGD,
        SPECTRUM = 0x200,
        PSA, PSV, PSD, SaRatio,
        CUMULATIVE = 0x300,
        Ia, DS575, DS595,
        ALL = 0x400,
    };

    Q_ENUM(Type)

    Type type;

};

}

// The combo name should be the key for the json file
class SimCenterIntensityMeasureCombo : public QComboBox
{
    Q_OBJECT

public:
    SimCenterIntensityMeasureCombo(SimCenterEQ::IntensityMeasure::Type imType, QString comboName, QWidget* parent = nullptr);

    void addParentItem(const QString& text);
    void addChildItem(const QString& text, const QVariant& data);

    QString getCurrentIMString(void);
    bool setCurrentIMString(const QString& unit);

    bool setCurrentIM(SimCenterEQ::IntensityMeasure::Type imType);

    QString getName() const;

signals:
    void IMChanged(QString newIM);

private:
    void populateComboText(const SimCenterEQ::IntensityMeasure::Type imType);

    template<typename IMEnum> QString imEnumToString(IMEnum enumValue);
    template<typename IMEnum> IMEnum imStringToEnum(QString imString);

    SimCenterEQ::IntensityMeasure::Type type;
    QString name;
};

#endif // SIMCENTERINTENSITYMEASURECOMBO_H
