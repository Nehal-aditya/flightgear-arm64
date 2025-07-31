//@file Expose aircraft related data to Nasal
//
// SPDX-FileCopyrightText: 2014 Thomas Geymayer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>

void initNasalAircraft(naRef globals, naContext c);
