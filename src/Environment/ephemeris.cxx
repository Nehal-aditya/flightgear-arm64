// ephemeris.cxx -- wrap SGEphemeris code in a subsystem
//
// Written by James Turner, started June 2010.
//
// SPDX-FileCopyrightText: 2010 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include <Environment/ephemeris.hxx>

#include <simgear/timing/sg_time.hxx>
#include <simgear/ephemeris/ephemeris.hxx>

#include <Main/globals.hxx>

static void tieStar(const char* prop, Star* s, double (Star::*getter)() const)
{
  fgGetNode(prop, true)->tie(SGRawValueMethods<Star, double>(*s, getter, NULL));
}

static void tieMoonPos(const char* prop, MoonPos* s, double (MoonPos::*getter)() const)
{
  fgGetNode(prop, true)->tie(SGRawValueMethods<MoonPos, double>(*s, getter, NULL));
}

Ephemeris::Ephemeris()
{
}

Ephemeris::~Ephemeris()
{
}

SGEphemeris* Ephemeris::data()
{
    return _impl.get();
}

void Ephemeris::init()
{
  SGPath ephem_data_path(globals->get_fg_root());
  ephem_data_path.append("Astro");
  _impl.reset(new SGEphemeris(ephem_data_path));

  tieStar("/ephemeris/sun/xs", _impl->get_sun(), &Star::getxs);
  tieStar("/ephemeris/sun/ys", _impl->get_sun(), &Star::getys);
  tieStar("/ephemeris/sun/ze", _impl->get_sun(), &Star::getze);
  tieStar("/ephemeris/sun/ye", _impl->get_sun(), &Star::getye);
  tieStar("/ephemeris/sun/lat-deg", _impl->get_sun(), &Star::getLat);

  tieMoonPos("/ephemeris/moon/xg", _impl->get_moon(), &MoonPos::getxg);
  tieMoonPos("/ephemeris/moon/yg", _impl->get_moon(), &MoonPos::getyg);
  tieMoonPos("/ephemeris/moon/ze", _impl->get_moon(), &MoonPos::getze);
  tieMoonPos("/ephemeris/moon/ye", _impl->get_moon(), &MoonPos::getye);
  tieMoonPos("/ephemeris/moon/lat-deg", _impl->get_moon(), &MoonPos::getLat);
  tieMoonPos("/ephemeris/moon/distance", _impl->get_moon(), &MoonPos::getDistance);
  tieMoonPos("/ephemeris/moon/phase", _impl->get_moon(), &MoonPos::getPhase);
  tieMoonPos("/ephemeris/moon/phase-angle", _impl->get_moon(), &MoonPos::getPhaseAngle);

    _latProp = fgGetNode("/position/latitude-deg", true);

    update(0.0);
}

void Ephemeris::shutdown()
{
    _impl.reset();
}

void Ephemeris::postinit()
{
}

void Ephemeris::bind()
{
}

void Ephemeris::unbind()
{
    _latProp = 0;
    _latProp.reset();
}

void Ephemeris::update(double)
{
    SGTime* st = globals->get_time_params();
    _impl->update(st->getMjd(), st->getLst(), _latProp->getDoubleValue());
}


// Register the subsystem.
SGSubsystemMgr::Registrant<Ephemeris> registrantEphemeris;
