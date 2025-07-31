// Add (std::string) like methods to Nasal strings
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>

naRef initNasalString(naRef globals, naRef string, naContext c);
