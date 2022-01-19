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

// Written: kuanshi

#include "math.h"
#include <algorithm>
#define PI 3.14159265

double BakerJayaram2008(double T1, double T2) {

    // min and max periods
    double Tmin = std::min(T1,T2);
    double Tmax = std::max(T1,T2);

    // coefficients
    double C1 = (1.0-cos(PI/2.0)-0.366*log(Tmax/std::max(Tmin,0.109)));
    double C2 = 1000.0;
    if (Tmax < 0.2) {
        C2 = 1.0-0.105*(1.0-1.0/(1.0+exp(100.0*Tmax-5.0)))*(Tmax-Tmin)/(Tmax-0.0099);
    }
    double C3 = C1;
    if (Tmax < 0.109) {
        C3 = C2;
    }
    double C4 = C1+0.5*(sqrt(C3)-C3)*(1.0+cos(PI*Tmin/0.109));

    // rho
    double rho = C2;
    if (Tmax > 0.109) {
        if (Tmin > 0.109) {
            rho = C1;
        } else {
            if (Tmax < 0.2) {
                rho = std::min(C2,C4);
            } else {
                rho = C4;
            }
        }
    }

    // return
    return rho;
}
