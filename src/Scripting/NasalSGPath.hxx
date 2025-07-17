// SPDX-FileCopyrightText: 2013 James Turner <james@flightgear.org>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Expose SGPath module to Nasal
 */

#pragma once

#include "simgear/misc/sg_path.hxx"
#include <simgear/nasal/nasal.h>

naRef initNasalSGPath(naRef globals, naContext c);

/**
 * @brief Map a string value such as `DESKTOP `to an SGPath location enum value.
 */
SGPath::StandardLocation standardLocationFromString(const std::string& s);
