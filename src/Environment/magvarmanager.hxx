// magvarmanager.hxx -- Wraps the SimGear SGMagVar in a subsystem
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/props/propsfwd.hxx>

// forward decls
class SGMagVar;

class FGMagVarManager : public SGSubsystem
{
public:
    FGMagVarManager();
    virtual ~FGMagVarManager();

    // Subsystem API.
    void bind() override;
    void init() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "magvar"; }

private:
    std::unique_ptr<SGMagVar> _magVar;

    SGPropertyNode_ptr _magVarNode, _magDipNode;
};
