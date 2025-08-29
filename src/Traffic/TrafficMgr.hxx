// TrafficMgr.hxx
//
// Written by Durk Talsma, started May 5, 2004.
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2004 Durk Talsma

/**************************************************************************
 * This file contains the class definitions for a (Top Level) traffic
 * manager for FlightGear.
 *
 * This is traffic manager version II. The major difference from version
 * I is that the Flight Schedules are decoupled from the AIAircraft
 * entities. This allows for a much greater flexibility in setting up
 * Irregular schedules. Traffic Manager II also makes no longer use of .xml
 * based configuration files.
 *
 * Here is a step plan to achieve the goal of creating Traffic Manager II
 *
 * 1) Read aircraft data from a simple text file, like the one provided by
 *    Gabor Toth
 * 2) Create a new database structure of SchedFlights. This new database
 *    should not be part of the Schedule class, but of TrafficManager itself
 * 3) Each aircraft should have a list of possible Flights it can operate
 *    (i.e. airline and AC type match).
 * 4) Aircraft processing proceeds as current. During initialization, we seek
 *    the most urgent flight that needs to be operated
 * 5) Modify the getNextLeg function so that the next flight is loaded smoothly.

 **************************************************************************/

#pragma once

#include <memory>
#include <set>

#include <simgear/misc/sg_path.hxx>
#include <simgear/props/propertyObject.hxx>
#include <simgear/structure/subsystem_mgr.hxx>

#include "SchedFlight.hxx"
#include "Schedule.hxx"


class Heuristic
{
public:
    std::string registration;
    unsigned int runCount;
    unsigned int hits;
    unsigned int lastRun;
};

typedef std::vector<Heuristic> heuristicsVector;
typedef std::vector<Heuristic>::iterator heuristicsVectorIterator;

typedef std::map<std::string, Heuristic> HeuristicMap;
typedef HeuristicMap::iterator HeuristicMapIterator;


class ScheduleParseThread;

class FGTrafficManager : public SGSubsystem
{
private:
    bool inited;
    bool doingInit;
    bool trafficSyncRequested;

    double waitingMetarTime;
    std::string waitingMetarStation;

    ScheduleVector scheduledAircraft;
    ScheduleVectorIterator currAircraft, currAircraftClosest;

    FGScheduledFlightMap flights;

    void readTimeTableFromFile(SGPath infilename);
    void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");

    simgear::PropertyObject<bool> enabled, aiEnabled, realWxEnabled, metarValid, active, aiDataUpdateNow;

    void loadHeuristics();

    bool doDataSync();
    void finishInit();
    void shutdown();

    friend class ScheduleParseThread;
    std::unique_ptr<ScheduleParseThread> scheduleParser;

    // helper to read and parse the schedule data.
    // this is run on a helper thread, so be careful about
    // accessing properties during parsing
    void parseSchedule(const SGPath& path);

    bool metarReady(double dt);

public:
    explicit FGTrafficManager();
    virtual ~FGTrafficManager();

    // Subsystem API.
    void init() override;
    void update(double time) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "traffic-manager"; }

    FGScheduledFlightVecIterator getFirstFlight(const std::string& ref) { return flights[ref].begin(); }
    FGScheduledFlightVecIterator getLastFlight(const std::string& ref) { return flights[ref].end(); }
};
