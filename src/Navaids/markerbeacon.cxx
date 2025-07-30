// markerbeacon.cxx -- marker beacon class
//
// SPDX-FileCopyrightText: 2008 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include <Navaids/markerbeacon.hxx>
#include <Airports/runways.hxx>
#include <Navaids/NavDataCache.hxx>

using std::string;

FGMarkerBeaconRecord::FGMarkerBeaconRecord(PositionedID aGuid, Type aTy,
                                           PositionedID aRunway, const SGGeod& aPos) :
  FGPositioned(aGuid, aTy, string(), aPos),
  _runway(aRunway)
{
}

FGRunwayRef FGMarkerBeaconRecord::runway() const
{
    FGPositioned* p = flightgear::NavDataCache::instance()->loadById(_runway);
    assert(p->type() == FGPositioned::RUNWAY);
    return static_cast<FGRunway*>(p);
}
