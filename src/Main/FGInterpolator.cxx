// Property interpolation manager for SGPropertyNodes
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FGInterpolator.hxx"
#include "fg_props.hxx"
#include <simgear/scene/util/ColorInterpolator.hxx>

//------------------------------------------------------------------------------
FGInterpolator::FGInterpolator()
{
  addInterpolatorFactory<simgear::ColorInterpolator>("color");

  setRealtimeProperty( fgGetNode("/sim/time/delta-realtime-sec", true) );
  SGPropertyNode::setInterpolationMgr(this);
}

//------------------------------------------------------------------------------
FGInterpolator::~FGInterpolator()
{
  if( SGPropertyNode::getInterpolationMgr() == this )
    SGPropertyNode::setInterpolationMgr(0);
}


// Register the subsystem.
SGSubsystemMgr::Registrant<FGInterpolator> registrantFGInterpolator(
    SGSubsystemMgr::INIT);
