// Airports forward declarations
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/SGSharedPtr.hxx>

#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime> // for time_t

// forward decls
class FGAirport;
class FGAirportDynamics;
class FGRunwayBase;
class FGRunway;
class FGHelipad;
class FGTaxiway;
class FGPavement;

class FGNavRecord;

class Block;
class FGTaxiNode;
class FGParking;
class FGTaxiSegment;
class FGTaxiRoute;
class FGGroundNetwork;

class RunwayList;
class RunwayGroup;
class FGRunwayPreference;

class FGSidStar;

class SGPropertyNode;

namespace flightgear {
  class SID;
  class STAR;
  class Approach;
  class Waypt;
  class CommStation;

  typedef std::vector<flightgear::SID*> SIDList;
  typedef std::vector<STAR*> STARList;
  typedef std::vector<Approach*> ApproachList;

  typedef SGSharedPtr<Waypt> WayptRef;
  typedef std::vector<WayptRef> WayptVec;

  typedef SGSharedPtr<CommStation> CommStationRef;
  typedef std::vector<CommStationRef> CommStationList;
  typedef std::map<std::string, FGAirport*> AirportCache;
}

typedef SGSharedPtr<FGAirport> FGAirportRef;
typedef SGSharedPtr<FGRunwayBase> FGRunwayBaseRef;
typedef SGSharedPtr<FGRunway> FGRunwayRef;
typedef SGSharedPtr<FGHelipad> FGHelipadRef;
typedef SGSharedPtr<FGTaxiway> FGTaxiwayRef;
typedef SGSharedPtr<FGPavement> FGPavementRef;
typedef SGSharedPtr<FGParking> FGParkingRef;
typedef SGSharedPtr<FGAirportDynamics> FGAirportDynamicsRef;

typedef std::vector<FGRunwayRef> FGRunwayList;
typedef std::map<std::string, FGRunwayRef> FGRunwayMap;
typedef std::map<std::string, FGHelipadRef> FGHelipadMap;

typedef std::vector<FGTaxiwayRef> FGTaxiwayList;
typedef std::vector<FGPavementRef> FGPavementList;
typedef std::vector<FGParkingRef> FGParkingList;

typedef std::vector<FGTaxiSegment*>  FGTaxiSegmentVector;
typedef FGTaxiSegmentVector::iterator FGTaxiSegmentVectorIterator;

typedef SGSharedPtr<FGTaxiNode> FGTaxiNodeRef;
typedef std::vector<FGTaxiNodeRef> FGTaxiNodeVector;
typedef FGTaxiNodeVector::iterator FGTaxiNodeVectorIterator;
typedef std::map<int, FGTaxiNodeRef> IndexTaxiNodeMap;

typedef std::vector<Block> BlockList;
typedef BlockList::iterator BlockListIterator;

typedef std::vector<time_t> TimeVector;
typedef std::vector<time_t>::iterator TimeVectorIterator;

typedef std::vector<FGTaxiRoute> TaxiRouteVector;
typedef std::vector<FGTaxiRoute>::iterator TaxiRouteVectorIterator;

typedef std::vector<RunwayList> RunwayListVec;
typedef std::vector<RunwayList>::iterator RunwayListVectorIterator;
typedef std::vector<RunwayList>::const_iterator RunwayListVecConstIterator;

typedef std::vector<RunwayGroup> PreferenceList;
typedef std::vector<RunwayGroup>::iterator PreferenceListIterator;
typedef std::vector<RunwayGroup>::const_iterator PreferenceListConstIterator;
