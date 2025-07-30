///@file Property interpolation manager for SGPropertyNodes
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/props/PropertyInterpolationMgr.hxx>

class FGInterpolator : public simgear::PropertyInterpolationMgr
{
public:
    FGInterpolator();
    ~FGInterpolator();

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "prop-interpolator"; }
};
