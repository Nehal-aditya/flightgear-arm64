// SPDX-FileCopyrightText: 2025 Stuart Buchanan
// SPDX-License-Identifier: GPL-2.0-or-later

#include "AircraftCompatibility.hxx"

#include <config.h>
#include <simgear/misc/strutils.hxx>
#include <simgear/props/props.hxx>

// Determine if a given aircraft is compatible with this FG version.
bool isAircraftCompatible(const SGPropertyNode_ptr props)
{
    // First check for minimum FG Version
    std::string minFGVersion = props->getStringValue("minimum-fg-version");
    if (!minFGVersion.empty()) {
        // If the minimum version to the first two elements is above this version, then we are incompatible.
        const int c = simgear::strutils::compare_versions(FLIGHTGEAR_VERSION, minFGVersion, 2);
        if (c < 0) return false;
    } else {
        // If the aircraft does not have a minimum FG version set, then we consider it incompatible.
        return false;
    }

    // Now check for specific compatibility flags required for this version.
    // Hardcoded, as these are explicitly dependent on the source code.
    if (props->hasChild("compatibility")) {
        auto compatibilityFlags = props->getChild("compatibility");
        if (!compatibilityFlags->getBoolValue("pbr-model")) return false;
    } else {
        // No compatibility flags means no compatibility
        return false;
    }

    //If we are here then we've passed the compatibility tests
    return true;
}
