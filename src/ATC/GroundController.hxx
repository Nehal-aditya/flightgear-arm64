// SPDX-FileCopyrightText: 2004 Durk Talsma
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/compiler.h>

#include <string>

#include <ATC/TowerController.hxx>
#include <ATC/trafficcontrol.hxx>

class FGAirportDynamics;
class AirportGroundRadar;


/**************************************************************************************
 * class FGGroundController
 *************************************************************************************/
class FGGroundController : public FGATCController
{
private:
    bool hasNetwork;
    bool networkInitialized;
    int count;
    int version;

    /**Returns the frequency to be used. */
    int getFrequency();


    void signOff(int id);
    void checkSpeedAdjustment(int id, double lat, double lon,
                              double heading, double speed, double alt);
    void checkHoldPosition(int id, double lat, double lon,
                           double heading, double speed, double alt);


    void updateStartupTraffic(TrafficVectorIterator i, int& priority, time_t now);
    bool updateActiveTraffic(TrafficVectorIterator i, int& priority, time_t now);

public:
    FGGroundController(FGAirportDynamics* par);
    ~FGGroundController();

    void setVersion(int v) { version = v; };
    int getVersion() { return version; };

    bool exists()
    {
        return hasNetwork;
    };

    virtual void announcePosition(int id, FGAIFlightPlan* intendedRoute, int currentRoute,
                                  double lat, double lon, double hdg, double spd, double alt,
                                  double radius, int leg, FGAIAircraft* aircraft);
    virtual void updateAircraftInformation(int id, SGGeod geod, double heading, double speed, double alt, double dt);
    virtual void handover(SGSharedPtr<FGTrafficRecord> aiObject, int leg);

    bool checkForCircularWaits(int id);
    virtual void render(bool);
    virtual std::string getName() const;
    virtual void update(double dt);

    void addVersion(int v) { version = v; };
};
