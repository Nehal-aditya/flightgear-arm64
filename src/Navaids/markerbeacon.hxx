// markerbeacon.hxx -- marker beacon class
//
// SPDX-FileCopyrightText: 2008 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/compiler.h>

#include "positioned.hxx"
#include <Airports/airports_fwd.hxx>

class FGMarkerBeaconRecord : public FGPositioned
{
public:

  FGMarkerBeaconRecord(PositionedID aGuid, Type aTy, PositionedID aRunway, const SGGeod& aPos);

    FGRunwayRef runway() const;

private:
    PositionedID _runway;
};
