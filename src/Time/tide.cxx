// tide.cxx -- interface for tidal movement
//
// Written by Erik Hofman, Octover 2020
//
// SPDX-FileCopyrightText: 2020 Erik Hofman <erik@ehofman.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <simgear/constants.h>
#include <simgear/timing/sg_time.hxx>
#include <simgear/structure/SGExpression.hxx>
#include <Main/globals.hxx>

#include "tide.hxx"
#include "light.hxx"
#include "bodysolver.hxx"

void FGTide::reinit() {
    _prev_moon_lon = -9999.0;
}

void FGTide::bind()
{
    SGPropertyNode *props = globals->get_props();

    viewLon = props->getNode("sim/current-view/viewer-lon-deg", true);
    viewLat = props->getNode("sim/current-view/viewer-lat-deg", true);

    _tideAnimation = props->getNode("/environment/sea/surface/delta-T-tide", true);

    _tideLevelNorm = props->getNode("/sim/time/tide-level-norm", true);
    _tideLevelNorm->setDoubleValue(_tide_level);
}

void FGTide::unbind()
{
    viewLon.reset();
    viewLat.reset();

    _tideLevelNorm.reset();
    _tideAnimation.reset();
}

#include <Main/fg_props.hxx>
void FGTide::update(double dt)
{
    auto l = globals->get_subsystem<FGLight>();

    // Don't know where the 60 degrees offset comes from but it matches
    // the tides perfectly at EHAL. Something to figure out.
    // Eureka: It was the latitude (53.45 degrees north).
    // It turns out that the moon is dragging the tide with an almost
    // perfect 45 degrees 'bow-wave' along the equator. Tests at SMBQ
    // (0 degrees latitude) confirmed this finding.
    double viewer_lon = (viewLon->getDoubleValue()
                         + fabs( viewLat->getDoubleValue() )
                        ) * SGD_DEGREES_TO_RADIANS;
    double moon_lon = l->get_moon_lon() - viewer_lon;
    if (fabs(_prev_moon_lon - moon_lon) > (SGD_PI/360.0))
    {
        _prev_moon_lon = moon_lon;

        double sun_lon = l->get_sun_lon() - viewer_lon;
        _tide_level = cos(2.0*moon_lon);
        _tide_level += 0.15*cos(2.0*sun_lon);

        if (_tide_level < -1.0) _tide_level = -1.0;
        else if (_tide_level > 1.0) _tide_level = 1.0;

        _tideLevelNorm->setDoubleValue(_tide_level);
        _tideAnimation->setDoubleValue(0.5 - 0.5*_tide_level);
    }
}

// Register the subsystem.
SGSubsystemMgr::Registrant<FGTide> registrantFGTide;
