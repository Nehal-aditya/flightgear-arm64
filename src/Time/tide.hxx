// tide.hxx -- interface for tidal movement
//
// Written by Erik Hofman, Octover 2020
//
// SPDX-FileCopyrightText: 2020 Erik Hofman <erik@ehofman.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <config.h>

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/props/tiedpropertylist.hxx>

class FGTide : public SGSubsystem
{
public:
    FGTide() = default;
    virtual ~FGTide() = default;

    // Subsystem API.
    void bind() override;
    void reinit() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "tides"; }

private:
    double _prev_moon_lon = -9999.0;
    double _tide_level = 0;

    SGPropertyNode_ptr viewLon;
    SGPropertyNode_ptr viewLat;
    SGPropertyNode_ptr _tideLevelNorm;
    SGPropertyNode_ptr _tideAnimation;
};
