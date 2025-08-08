// terrainsampler.hxx --
//
// Written by Torsten Dreyer, started July 2010
// Based on local weather implementation in nasal from
// Thorsten Renk
//
// SPDX-FileCopyrightText: 2010 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>

namespace Environment {

class TerrainSampler : public SGSubsystemGroup
{
public:
    static TerrainSampler * createInstance( SGPropertyNode_ptr rootNode );
};

} // namespace Environment
