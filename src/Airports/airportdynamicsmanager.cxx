// airportdynamicsmanager.cxx - manager for dynamic (changeable)
// part of airport state
//
// Written by James Turner, started December 2015
//
// SPDX-FileCopyrightText: 2015 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include "airportdynamicsmanager.hxx"

#include <simgear/structure/exception.hxx>

#include "airport.hxx"
#include "xmlloader.hxx"
#include "dynamics.hxx"
#include "runwayprefs.hxx"

#include <Main/globals.hxx>

namespace flightgear
{

AirportDynamicsManager::AirportDynamicsManager()
{

}


AirportDynamicsManager::~AirportDynamicsManager()
{


}

void AirportDynamicsManager::init()
{


}

void AirportDynamicsManager::shutdown()
{
    m_dynamics.clear();
}

void AirportDynamicsManager::update(double dt)
{
    SG_UNUSED(dt);
}

void AirportDynamicsManager::reinit()
{
    shutdown();
    init();
}

FGAirportDynamicsRef AirportDynamicsManager::dynamicsForICAO(const std::string &icao)
{
    ICAODynamicsDict::iterator it = m_dynamics.find(icao);
    if (it != m_dynamics.end()) {
        return it->second;
    }

    FGAirportRef apt(FGAirport::findByIdent(icao));
    if (!apt)
        return FGAirportDynamicsRef();

    FGAirportDynamicsRef d(new FGAirportDynamics(apt));
    d->init();

    FGRunwayPreference rwyPrefs(apt);
    XMLLoader::load(&rwyPrefs);
    d->setRwyUse(rwyPrefs);

    m_dynamics[icao] = d;
    return d;
}

FGAirportDynamicsRef AirportDynamicsManager::find(const std::string &icao)
{
    if (icao.empty())
        return FGAirportDynamicsRef();

    auto instance = globals->get_subsystem<AirportDynamicsManager>();
    if (!instance)
        return FGAirportDynamicsRef();

    return instance->dynamicsForICAO(icao);
}

FGAirportDynamicsRef AirportDynamicsManager::find(const FGAirportRef& apt)
{
    if (!apt)
        return {};

    return find(apt->ident());
}

// Register the subsystem.
SGSubsystemMgr::Registrant<AirportDynamicsManager> registrantAirportDynamicsManager;

} // of namespace flightgear
