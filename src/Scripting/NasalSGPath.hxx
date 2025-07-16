//@file Expose SGPath module to Nasal

// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2013  James Turner - james@flightgear.org

#pragma once

#include "simgear/misc/sg_path.hxx"
#include <simgear/nasal/nasal.h>

naRef initNasalSGPath(naRef globals, naContext c);

/**
 * @brief map a string value such as 'DESKTOP' to a SGPath location enum value
 *
 * @param s
 * @return SGPath::StandardLocation
 */
SGPath::StandardLocation standardLocationFromString(const std::string& s);
