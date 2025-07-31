// Expose aircraft related data to Nasal
//
// SPDX-FileCopyrightText: 2014 Thomas Geymayer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "NasalAircraft.hxx"
#include <Aircraft/FlightHistory.hxx>
#include <Main/globals.hxx>

#include <simgear/nasal/cppbind/NasalHash.hxx>
#include <simgear/nasal/cppbind/Ghost.hxx>

//------------------------------------------------------------------------------
static naRef f_getHistory(const nasal::CallContext& ctx)
{
  auto history = globals->get_subsystem<FGFlightHistory>();
  if( !history )
    ctx.runtimeError("Failed to get 'history' subsystem");

  return ctx.to_nasal(history);
}

//------------------------------------------------------------------------------
void initNasalAircraft(naRef globals, naContext c)
{
  nasal::Ghost<SGSharedPtr<FGFlightHistory> >::init("FGFlightHistory")
    .method("pathForHistory", &FGFlightHistory::pathForHistory);

  nasal::Hash aircraft_module = nasal::Hash(globals, c).createHash("aircraft");
  aircraft_module.set("history", &f_getHistory);
}
