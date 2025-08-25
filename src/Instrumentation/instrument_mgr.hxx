// instrument_mgr.hxx - manage aircraft instruments.
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2002 David Megginson (public domain)

#pragma once

#include <simgear/compiler.h>
#include <simgear/structure/subsystem_mgr.hxx>


/**
 * Manage aircraft instruments.
 *
 * In the initial draft, the instruments present are hard-coded, but they
 * will soon be configurable for individual aircraft.
 */
class FGInstrumentMgr : public SGSubsystemGroup
{
public:
    FGInstrumentMgr ();
    virtual ~FGInstrumentMgr ();

    // Subsystem API.
    void init() override;
    InitStatus incrementalInit() override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "instrumentation"; }

private:
    bool build (SGPropertyNode* config_props, const SGPath& path);

    bool _explicitGps = false;

    std::vector<std::string> _instruments;
};
