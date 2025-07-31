// NasalPositioned.hxx -- expose FGPositioned classes to Nasal
//
// Written by James Turner, started 2012.
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>

#include <Navaids/positioned.hxx>


// forward decls
class SGGeod;
class FGRunway;
class FGAirport;

bool geodFromHash(naRef ref, SGGeod& result);

FGAirport* airportGhost(naRef r);
FGRunway* runwayGhost(naRef r);

naRef ghostForPositioned(naContext c, FGPositionedRef pos);
naRef ghostForRunway(naContext c, const FGRunway* r);
naRef ghostForAirport(naContext c, const FGAirport* apt);

FGPositioned* positionedGhost(naRef r);
FGPositionedRef positionedFromArg(naRef ref);
int geodFromArgs(naRef* args, int offset, int argc, SGGeod& result);

naRef initNasalPositioned(naRef globals, naContext c);
naRef initNasalPositioned_cppbind(naRef globals, naContext c);
void postinitNasalPositioned(naRef globals, naContext c);
void shutdownNasalPositioned();
