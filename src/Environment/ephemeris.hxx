// ephemeris.hxx -- wrap SGEphemeris code in a subsystem
//
// Written by James Turner, started June 2010.
//
// SPDX-FileCopyrightText: 2010 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>

#include <Main/fg_props.hxx>

class SGEphemeris;
class SGPropertyNode;

/**
 * Wrap SGEphemeris in a subsystem/property interface
 */
class Ephemeris : public SGSubsystem
{
public:
    Ephemeris();
    ~Ephemeris();

    // Subsystem API.
    void bind() override;
    void init() override;
    void postinit() override;
    void shutdown() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "ephemeris"; }

    SGEphemeris* data();

private:
    std::unique_ptr<SGEphemeris> _impl;
    SGPropertyNode_ptr _latProp;
};
