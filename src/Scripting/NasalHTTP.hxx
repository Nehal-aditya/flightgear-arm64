//@file Expose HTTP module to Nasal
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>

naRef initNasalHTTP(naRef globals, naContext c);
