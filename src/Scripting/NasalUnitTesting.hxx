/*
 * SPDX-FileComment: Unit-test API for nasal
 * SPDX-FileCopyrightText: 2020 James Turner
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <Scripting/NasalSys.hxx>

class SGPath;

naRef initNasalUnitTestInSim(naRef globals, naContext c);

void shutdownNasalUnitTestInSim();

void executeNasalTestsInDir(const SGPath& path);
bool executeNasalTest(const SGPath& path);
