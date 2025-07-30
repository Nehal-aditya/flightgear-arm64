// airportdynamicsmanager.hxx - manager for dynamic (changeable)
// part of airport state
//
// Written by James Turner, started December 2015
//
// SPDX-FileCopyrightText: 2015 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/structure/SGSharedPtr.hxx>

#include <map>

#include "airports_fwd.hxx"

namespace flightgear
{

class AirportDynamicsManager : public SGSubsystem
{
public:
    AirportDynamicsManager();
    virtual ~AirportDynamicsManager();

    // Subsystem API.
    void init() override;
    void reinit() override;
    void shutdown() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "airport-dynamics"; }

    static FGAirportDynamicsRef find(const std::string& icao);

    static FGAirportDynamicsRef find(const FGAirportRef& apt);

    FGAirportDynamicsRef dynamicsForICAO(const std::string& icao);

private:
    typedef std::map<std::string, FGAirportDynamicsRef> ICAODynamicsDict;
    ICAODynamicsDict m_dynamics;
};

} // namespace flightgear
