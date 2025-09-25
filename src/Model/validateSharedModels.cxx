// SPDX-FileCopyrightText: 2025 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include <Model/validateSharedModels.hxx>

// SimGear
#include <simgear/misc/ResourceManager.hxx>

#include <GUI/MessageBox.hxx>
#include <Main/fg_props.hxx>
#include <Main/globals.hxx>
#include <Main/locale.hxx>

namespace flightgear {

static bool static_haveShownError = false;

bool validateSharedModels()
{
    string_list paths = {
        "Models/Airport/marker.ac",
        "Models/Airport/beacon.xml",
        "Models/Airport/localizer.xml",
        "Models/Airport/windsock_lit.xml",
        "Models/Misc/trigpoint.ac",
        "Models/Industrial/generic_chimney_01.xml",
        "Models/Airport/Vehicle/Cobus_3000.xml",
        "Models/Industrial/GenericStorageTank40m.ac",
        "Models/Boundaries/Fence_50m.ac",
        "Models/Residential/french_house_s.xml",
        "Models/Power/generic_pylon_50m.ac"};

    bool missing = false;
    for (auto p : paths) {
        SGPath ex = simgear::ResourceManager::instance()->findPath(p);
        if (!ex.exists()) {
            missing = true;
            break;
        }
    }

    // exit here if all the test paths were found correctly
    if (!missing) {
        return true; // validate succeeded
    }

    if (static_haveShownError) {
        return false; // still return the status, but no need to show again (eg, re-init)
    }

    const auto usingTerraSync = fgGetBool("/sim/terrasync/enabled");
    const auto title = globals->get_locale()->getLocalizedString("shared-models-missing-title", "sys");
    const auto msg = globals->get_locale()->getLocalizedString(usingTerraSync ? "shared-models-missing-terrasync" : "shared-models-missing", "sys");
    modalMessageBox(title, msg);
    static_haveShownError = true;
    return false; // validate failed
}

bool haveShownSharedModelsError()
{
    return static_haveShownError;
}

} // namespace flightgear
