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

#include "EarthquakeEventSelection.h"
#include <GoogleAnalytics.h>
#include <RandomVariablesContainer.h>
#include <GeneralInformationWidget.h>

#include <InputWidgetExistingEvent.h>
#include <ExistingSimCenterEvents.h>
#include <ExistingPEER_Records.h>
//#include <UserDefinedApplication.h>
#include "StochasticMotionInput.h"
#include "RockOutcrop.h"
#include "peerNGA/PEER_NGA_Records.h"
#include "userDefinedDatabase/User_Defined_Database.h"
#include "physicsBasedSimulation/PhysicsBasedMotionSelection.h"
#include "drmEvent/drmEvent.h"
#include "SimCenterAppMulti.h"

EarthquakeEventSelection::EarthquakeEventSelection(RandomVariablesContainer *theRandomVariables,
						   GeneralInformationWidget* generalInfo, bool doMulti,
						   QWidget *parent)
  : SimCenterEventAppSelection("Load Generator", "Events", "Earthquake", parent), theRVs(theRandomVariables), theGI(generalInfo)
{
  
    StochasticMotionInput *theStochasticMotionWidget = new StochasticMotionInput(theRandomVariables);
    PEER_NGA_Records *peerNgaRecords = new PEER_NGA_Records(generalInfo, this);
    RockOutcrop *theRockOutcrop = new RockOutcrop(theRandomVariables);
    ExistingPEER_Records *theExistingPeerEvents = new ExistingPEER_Records(theRandomVariables);
    ExistingSimCenterEvents *theExistingEvents = new ExistingSimCenterEvents(theRandomVariables);
    User_Defined_Database *userDefinedDatabase = new User_Defined_Database(generalInfo, this);
    PhysicsBasedMotionSelection *physicsBased = new PhysicsBasedMotionSelection(generalInfo, this);

    // upper case below need to move to tools
    this->addComponent(tr("Stochastic Ground Motion"),"StochasticGroundMotion", theStochasticMotionWidget);
    this->addComponent(tr("PEER NGA Records"), "PEER NGA RECORDS", peerNgaRecords);
    this->addComponent(tr("Site Response"), "Site Response", theRockOutcrop);
    this->addComponent(tr("Multiple PEER"), "ExistingPEER_Events", theExistingPeerEvents);
    this->addComponent(tr("Multiple SimCenter"), "ExistingSimCenterEvents", theExistingEvents);
    this->addComponent(tr("User Specified Database"), "USER_DEFINED_DATABASE", userDefinedDatabase);
    this->addComponent(tr("Physics Based Simulations"), "PhysicsBasedMotion", physicsBased);
    DRMevent *drmEventWidget = new DRMevent(this);
    this->addComponent(tr("DRM Event"), "DRM", drmEventWidget);

  if (doMulti == true) {
    SimCenterAppWidget *multi = new SimCenterAppMulti(QString("Events"), QString("MultiModel-Events"),this, this);
    this->addComponent(QString("Multiple Models"), QString("MultiModel"), multi);
  }        
}

EarthquakeEventSelection::~EarthquakeEventSelection()
{

}

void
EarthquakeEventSelection::replyEventType(void)
{
    emit typeEVT("EQ");
  //qDebug() << "EarthquakeEventSelection::replyEventType(void)- HUH";
  
}

SimCenterAppWidget *
EarthquakeEventSelection::getClone()
{
  EarthquakeEventSelection *newSelection = new EarthquakeEventSelection(theRVs, theGI, false);
  return newSelection;
}
