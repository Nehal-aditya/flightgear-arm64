// positioninit.hxx - helpers relating to setting initial aircraft position
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace flightgear
{

// Set the initial position based on presets (or defaults)
bool initPosition();

// Listen to /sim/tower/airport-id and set tower view position accordingly
void initTowerLocationListener();

// allow this to be manually invoked for position init testing
void finalizePosition();

} // namespace flightgear
