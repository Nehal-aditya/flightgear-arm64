// main.hxx -- top level sim routines
//
// Written by Curtis Olson, started May 1997.
//
// SPDX-FileCopyrightText: 1997 Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

int fgMainInit( int argc, char **argv );

void fgResetIdleState();

extern std::string hostname;

namespace flightgear {

void registerMainLoop();
void unregisterMainLoopProperties();

} // namespace flightgear
