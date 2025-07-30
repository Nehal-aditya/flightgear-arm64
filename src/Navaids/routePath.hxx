/**
 * routePath.hxx - convert a route to straight line segments, for graphical
 * output or display.
 */

// SPDX-FileCopyrightText: 2018 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <Navaids/route.hxx>

namespace flightgear
{
  class Hold;
  class FlightPlan;
  class Via;

  typedef std::vector<SGGeod> SGGeodVec;
}

class RoutePath
{
public:
  RoutePath(const flightgear::FlightPlan* fp);
  ~RoutePath();

  RoutePath(const RoutePath& other);
  RoutePath& operator=(const RoutePath& other);

  flightgear::SGGeodVec pathForIndex(int index) const;

  SGGeod positionForIndex(int index) const;

  SGGeod positionForDistanceFrom(int index, double distanceM) const;

  double trackForIndex(int index) const;

  double distanceForIndex(int index) const;

  double distanceBetweenIndices(int from, int to) const;

private:
  class RoutePathPrivate;

  void commonInit();

  double computeDistanceForIndex(int index) const;

  double distanceForVia(flightgear::Via *via, int index) const;


  flightgear::SGGeodVec pathForHold(flightgear::Hold* hold) const;
  flightgear::SGGeodVec pathForVia(flightgear::Via* via, int index) const;
  SGGeod positionAlongVia(flightgear::Via* via, int previousIndex, double distanceM) const;

  void interpolateGreatCircle(const SGGeod& aFrom, const SGGeod& aTo,
                              flightgear::SGGeodVec& r) const;


  std::unique_ptr<RoutePathPrivate> d;
};
