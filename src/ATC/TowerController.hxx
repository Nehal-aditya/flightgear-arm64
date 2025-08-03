// SPDX-FileCopyrightText: 2006 Durk Talsma
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <Airports/airports_fwd.hxx>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Shape>

#include <simgear/compiler.h>
#include <simgear/constants.h>
#include <simgear/debug/logstream.hxx>
#include <simgear/structure/SGReferenced.hxx>
#include <simgear/structure/SGSharedPtr.hxx>

#include <ATC/ATCController.hxx>
#include <ATC/trafficcontrol.hxx>

/******************************************************************************
 * class FGTowerControl
 *****************************************************************************/
class FGTowerController : public FGATCController
{
private:
    /**Returns the frequency to be used. */
    int getFrequency();

public:
    FGTowerController(FGAirportDynamics* parent);
    virtual ~FGTowerController();

    virtual void announcePosition(int id, FGAIFlightPlan* intendedRoute, int currentRoute,
                                  double lat, double lon,
                                  double hdg, double spd, double alt, double radius, int leg,
                                  FGAIAircraft* aircraft);
    void signOff(int id);
    bool hasInstruction(int id);
    FGATCInstruction getInstruction(int id);
    virtual void updateAircraftInformation(int id, SGGeod geod,
                                           double heading, double speed, double alt, double dt);

    virtual void render(bool);
    virtual std::string getName() const;
    virtual void update(double dt);
};
