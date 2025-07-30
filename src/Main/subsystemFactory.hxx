// subsystemFactory.hxx - factory for subsystems
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

// forward decls
class SGCommandMgr;
class SGSubsystem;

namespace flightgear
{

/**
 * create a subsystem by name, and return it.
 * Will throw an exception if the subsystem name is invalid, or
 * if the subsystem could not be created for some other reason.
 * ownership of the subsystem passes to the caller
 */
SGSubsystem* createSubsystemByName(const std::string& name);

void registerSubsystemCommands(SGCommandMgr* cmdMgr);

} // namespace flightgear
