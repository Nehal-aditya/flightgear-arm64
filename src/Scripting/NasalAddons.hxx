// -*- coding: utf-8 -*-
//
// NasalAddons.hxx --- Expose add-on classes to Nasal
// SPDX-FileCopyrightText: 2017 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>

namespace flightgear
{

namespace addons
{

void initAddonClassesForNasal(naRef globals, naContext c);

} // of namespace addons

} // namespace flightgear
