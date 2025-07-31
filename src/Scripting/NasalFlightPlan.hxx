// NasalFlightPlan.hxx -- expose FlightPlan classes to Nasal
//
// Written by James Turner, started 2020.
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2020 James Turner <james@flightgear.org>

#pragma once

#include <simgear/nasal/nasal.h>

#include <Navaids/FlightPlan.hxx>
#include <Navaids/procedure.hxx>

flightgear::Waypt*           wayptGhost(naRef r);
flightgear::FlightPlan::Leg* fpLegGhost(naRef r);
flightgear::Procedure*       procedureGhost(naRef r);

naRef ghostForWaypt(naContext c, const flightgear::Waypt* wpt);
naRef ghostForLeg(naContext c, const flightgear::FlightPlan::Leg* leg);
naRef ghostForProcedure(naContext c, const flightgear::Procedure* proc);

naRef initNasalFlightPlan(naRef globals, naContext c);
void  shutdownNasalFlightPlan();
