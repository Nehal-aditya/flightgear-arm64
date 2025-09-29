/******************************************************************************
 * AIFlightPlanCreateCruise.cxx
 * Written by Durk Talsma, started February, 2006.
 *
 * SPDX-FileCopyrightText: Written by Durk Talsma, started February, 2006
 * SPDX-License-Identifier: GPL-2.0-or-later
 **************************************************************************/

#include <config.h>

#include <fstream>

#include <Airports/airport.hxx>
#include <Airports/runways.hxx>
#include <Airports/dynamics.hxx>

#include <Environment/environment_mgr.hxx>
#include <Environment/environment.hxx>

#include <Traffic/Schedule.hxx>

#include "AIFlightPlan.hxx"
#include "AIAircraft.hxx"
#include "performancedata.hxx"

using std::string;

/*******************************************************************
 * CreateCruise
 * initialize the Aircraft at the parking location
 *
 * Note that this is the original version that does not
 * do any dynamic route computation.
 ******************************************************************/
bool FGAIFlightPlan::createCruise(FGAIAircraft *ac, bool firstFlight, FGAirport *dep,
                  FGAirport *arr,
                  const SGGeod& current,
                  double speed,
                  double alt,
                  const string& fltType)
{
  double vCruise = ac->getPerformance()->vCruise();
  FGAIWaypoint *wpt;
  //FIXME usually that will be "before" the next WP
  wpt = createInAir(ac, "Cruise", current, alt, vCruise);
  if (waypoints.size() == 0) {
    pushBackWaypoint(wpt);
    SG_LOG(SG_AI, SG_DEBUG, "Cruise spawn " << ac->getCallSign());
  } else {
    SG_LOG(SG_AI, SG_DEBUG, "Cruise start " << ac->getCallSign());
  }
  //

  const string& rwyClass = getRunwayClassFromTrafficType(fltType);
  double heading = ac->getTrafficRef()->getCourse();
  arr->getDynamics()->getActiveRunway(rwyClass, RunwayAction::LANDING, activeRunway, heading);
  if (!arr->hasRunwayWithIdent(activeRunway)) {
    SG_LOG(SG_AI, SG_WARN, ac->getCallSign() << " cruise to" << arr->getId() << activeRunway << " not active");
    return false;
  }

  FGRunway* rwy = arr->getRunwayByIdent(activeRunway);
  assert( rwy != NULL );
  // begin descent 110km out
  double distanceOut = arr->getDynamics()->getRunwayQueue(rwy->name())->getApproachDistance();    //12 * SG_NM_TO_METER;

  SGGeod beginDescentPoint     = rwy->pointOnCenterline(-3*distanceOut);
  SGGeod secondaryDescentPoint = rwy->pointOnCenterline(0);

  double distanceToRwy = SGGeodesy::distanceM(current, secondaryDescentPoint);
  if (distanceToRwy>4*distanceOut) {
    FGAIWaypoint *bodWpt = createInAir(ac, "BOD", beginDescentPoint,  alt, vCruise);
    pushBackWaypoint(bodWpt);
    FGAIWaypoint *bod2Wpt = createInAir(ac, "BOD2", secondaryDescentPoint, alt, vCruise);
    pushBackWaypoint(bod2Wpt);
  } else {
    // We are too near. The descent leg takes care of this (teardrop etc)
    FGAIWaypoint *bodWpt = createInAir(ac, "BOD", SGGeodesy::direct(current, ac->getTrueHeadingDeg(), 10000),  alt, vCruise);
    pushBackWaypoint(bodWpt);
    FGAIWaypoint *bod2Wpt = createInAir(ac, "BOD2", SGGeodesy::direct(current, ac->getTrueHeadingDeg(), 15000), alt, vCruise);
    pushBackWaypoint(bod2Wpt);
  }
  return true;
}
