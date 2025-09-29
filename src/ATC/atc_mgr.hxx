// SPDX-FileCopyrightText: 2010 Durk Talsma
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/SGReferenced.hxx>
#include <simgear/structure/subsystem_mgr.hxx>

#include <AIModel/AIAircraft.hxx>
#include <ATC/EnRouteController.hxx>
#include <ATC/trafficcontrol.hxx>
#include <Traffic/SchedFlight.hxx>
#include <Traffic/Schedule.hxx>


typedef std::vector<FGATCController*> AtcVec;
typedef std::vector<FGATCController*>::iterator AtcVecIterator;

/**
 * The ATC Manager interfaces the users aircraft within the AI traffic system
 * and also monitors the ongoing AI traffic patterns for potential conflicts
 * and interferes where necessary.
 */
class FGATCManager : public SGSubsystem
{
private:
    AtcVec activeStations;
    FGATCController *controller, *prevController; // The ATC controller that is responsible for the user's aircraft.
    FGATCController* enRouteController;

    bool networkVisible;
    bool initSucceeded;
    SGPropertyNode_ptr trans_num;
    std::string destination;

    std::unique_ptr<FGAISchedule> userAircraftTrafficRef;
    std::unique_ptr<FGScheduledFlight> userAircraftScheduledFlight;

    SGPropertyNode_ptr _routeManagerDestinationAirportNode;

public:
    FGATCManager();
    virtual ~FGATCManager();

    // Subsystem API.
    void postinit() override;
    void shutdown() override;
    void update(double time) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "ATC"; }

    void addController(FGATCController* controller);
    void removeController(FGATCController* controller);

    FGATCController* getEnRouteController();

    void reposition();
};
