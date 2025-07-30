/*
 * SPDX-FileComment: manage aircraft systems
 * SPDX-FileCopyrightText: 2002 David Megginson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <simgear/compiler.h>
#include <simgear/props/propsfwd.hxx>
#include <simgear/structure/subsystem_mgr.hxx>

/**
 * Manage aircraft systems.
 *
 * Multiple aircraft systems can be configured for each aircraft.
 */
class FGSystemMgr : public SGSubsystemGroup
{
public:
    FGSystemMgr ();
    virtual ~FGSystemMgr ();

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "systems"; }

    bool build (SGPropertyNode* config_props);
};
