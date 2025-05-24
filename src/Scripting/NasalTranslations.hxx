// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Translations: Nasal interface to FGTranslate and related classes
          (header file)
 */

#pragma once

#include <simgear/nasal/nasal.h>

namespace flightgear
{

void initNasalTranslations(naRef globals, naContext c);

} // of namespace flightgear
