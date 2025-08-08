// magvarmanager.cxx -- Wraps the SimGear SGMagVar in a subsystem
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include <config.h>

#include "magvarmanager.hxx"

#include <simgear/sg_inlines.h>
#include <simgear/magvar/magvar.hxx>
#include <simgear/timing/sg_time.hxx>
#include <simgear/math/SGMath.hxx>

#include <Main/globals.hxx>
#include <Main/fg_props.hxx>

FGMagVarManager::FGMagVarManager() :
    _magVar(new SGMagVar)
{
}

FGMagVarManager::~FGMagVarManager()
{
}

void FGMagVarManager::init()
{
    update(0.0); // force an immediate update
}

void FGMagVarManager::bind()
{
  _magVarNode = fgGetNode("/environment/magnetic-variation-deg", true);
  _magDipNode = fgGetNode("/environment/magnetic-dip-deg", true);
}

void FGMagVarManager::unbind()
{
  _magVarNode = SGPropertyNode_ptr();
  _magDipNode = SGPropertyNode_ptr();
}

void FGMagVarManager::update(double dt)
{
  SG_UNUSED(dt);

  // update magvar model
  _magVar->update( globals->get_aircraft_position(),
                     globals->get_time_params()->getJD() );

  _magVarNode->setDoubleValue(_magVar->get_magvar() * SG_RADIANS_TO_DEGREES);
  _magDipNode->setDoubleValue(_magVar->get_magdip() * SG_RADIANS_TO_DEGREES);
}


// Register the subsystem.
SGSubsystemMgr::Registrant<FGMagVarManager> registrantFGMagVarManager;
