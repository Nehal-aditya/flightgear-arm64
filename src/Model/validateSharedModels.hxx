// SPDX-FileCopyrightText: 2025 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace flightgear {

/**
 * @brief check if the Shared models are available correctly.
 *
 * Will show a fatal error message box if they're not found. We don't check for
 * every model, but for a representative subset which gives us a good
 * confidence that the user did get a working Models/ directory from TerraSync
 * or elsewhere.
 *
 */
bool validateSharedModels();

/// helper to avoid informing the user redundantly about missing models
bool haveShownSharedModelsError();

} // namespace flightgear
