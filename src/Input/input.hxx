// input.hxx -- handle user input from various sources.
//
// Written by David Megginson, started May 2001.
// Major redesign by Torsten Dreyer, started August 2009
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson <david@megginson.com>
// SPDX-FileCopyrightText: 2009 Torsten Dreyer

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>


////////////////////////////////////////////////////////////////////////
// General input mapping support.
////////////////////////////////////////////////////////////////////////


/**
 * Generic input module.
 *
 * <p>This module is designed to handle input from multiple sources --
 * keyboard, joystick, mouse, or even panel switches -- in a consistent
 * way, and to allow users to rebind any of the actions at runtime.</p>
 */
class FGInput : public SGSubsystemGroup
{
public:
    /**
     * Default constructor.
     */
    FGInput ();

    /**
     * Destructor.
     */
    virtual ~FGInput();

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "input"; }
};
