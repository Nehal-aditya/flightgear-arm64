// parking.cxx - Implementation of a class to manage aircraft parking in
// FlightGear. This code is intended to be used by AI code and
// initial user-startup location selection.
//
// Written by Durk Talsma, started December 2004.
//
// SPDX-FileCopyrightText: 2004 Durk Talsma
// SPDX-License-Identifier: GPL-2.0-or-later

#include <config.h>

#include <simgear/compiler.h>

#include <string>

#include "parking.hxx"

/*********************************************************************************
 * FGParking
 ********************************************************************************/

FGParking::FGParking(int index,
                     const SGGeod& pos,
                     double aHeading, double aRadius,
                     const std::string& name,
                     const std::string& aType,
                     const std::string& codes) : FGTaxiNode(FGPositioned::PARKING, index, pos, false, 0, name),
                                                 heading(aHeading),
                                                 reverseHeading(fmod(aHeading + 180.0, 360.0)),
                                                 radius(aRadius),
                                                 type(aType),
                                                 airlineCodes(codes)
{
}

void FGParking::setPushBackPoint(const FGTaxiNodeRef& node)
{
    pushBackPoint = node;
}
