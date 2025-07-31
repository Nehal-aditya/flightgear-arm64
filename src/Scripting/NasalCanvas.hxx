// NasalCanvas.hxx -- expose Canvas classes to Nasal
//
// Written by James Turner, started 2012.
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>

naRef initNasalCanvas(naRef globals, naContext c);
