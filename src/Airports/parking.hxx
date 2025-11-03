// parking.hxx - A class to handle airport startup locations in
// FlightGear. This code is intended to be used by AI code and
// initial user-startup location selection.
//
// Written by Durk Talsma, started December 2004.
//
// SPDX-FileCopyrightText: 2004 Durk Talsma
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/compiler.h>
#include <simgear/sg_inlines.h>

#include <string>

#include "gnnode.hxx"
#include <Airports/airports_fwd.hxx>

class FGParking : public FGTaxiNode
{
private:
  const double heading;
  const double reverseHeading;
  const double radius;
  const std::string type;
  const std::string airlineCodes;
  FGTaxiNodeRef pushBackPoint;

  SG_DISABLE_COPY(FGParking);
public:
  static bool isType(FGPositioned::Type ty)
  { return (ty == FGPositioned::PARKING); }

  FGParking(int index,
            const SGGeod& pos,
            double heading, double radius,
            const std::string& name, const std::string& type,
            const std::string& codes);
    virtual ~FGParking() = default;

    double getHeading() const { return heading; };
    double getReverseHeading() const { return reverseHeading; };
    double getRadius() const { return radius; };

    std::string getType() const { return type; };
    std::string getCodes() const { return airlineCodes; };
    std::string getName() const { return ident(); };

    void setPushBackPoint(const FGTaxiNodeRef& node);
    FGTaxiNodeRef getPushBackPoint() { return pushBackPoint; };

    bool operator<(const FGParking& other) const
    {
        return radius < other.radius;
    };
};
