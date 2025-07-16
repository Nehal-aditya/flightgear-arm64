/*
 * SPDX-FileName: testStringUtils.hxx
 * SPDX-FileCopyrightText: Copyright (C) 2025 Keith Paterson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <simgear/math/SGGeod.hxx>
#include <simgear/props/propsfwd.hxx>
#include <simgear/structure/SGSharedPtr.hxx>

namespace FGTestApi {

namespace strings {

/**Returns a string for traffic refs */
std::string getTimeString(int timeOffset);
} // namespace strings
} // End of namespace FGTestApi.
