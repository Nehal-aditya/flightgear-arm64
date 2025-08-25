/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * SPDX-FileCopyrightText: 2000 Curtis L. Olson - http://www.flightgear.org/~curt
 * SPDX-FileCopyrightText: 2011 Torsten Dreyer
 *
 * navradio.hxx -- class to manage a nav radio instance
 * Written by Torsten Dreyer, started August 2011
*/

#pragma once

#include <simgear/props/props.hxx>
#include <simgear/structure/subsystem_mgr.hxx>

namespace Instrumentation {

class NavRadio : public SGSubsystem
{
public:
    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "nav-radio"; }

    static SGSubsystem * createInstance( SGPropertyNode_ptr rootNode );
};

}
