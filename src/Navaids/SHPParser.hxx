/**
 * SHPParser - parse ESRI ShapeFiles containing PolyLines */

// Written by James Turner, started 2013.
//
// SPDX-FileCopyrightText: 2013 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <Navaids/PolyLine.hxx>

// forward decls
class SGPath;

namespace flightgear
{

class SHPParser
{
public:
    /**
     * Parse a shape file containing PolyLine data.
     *
     * Throws sg_exceptions if parsing problems occur.
     */
    static void parsePolyLines(const SGPath&, PolyLine::Type aTy, PolyLineList& aResult, bool aClosed);
};

} // namespace flightgear
