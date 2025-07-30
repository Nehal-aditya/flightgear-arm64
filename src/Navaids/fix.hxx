// fix.hxx -- fix class
//
// Written by Curtis Olson, started April 2000.
//
// SPDX-FileCopyrightText: 2000 Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/compiler.h>

#include <string>

#include "positioned.hxx"

class FGFix : public FGPositioned
{
public:
  FGFix(PositionedID aGuid, const std::string& aIdent, const SGGeod& aPos);
  inline ~FGFix(void) {}

  static bool isType(FGPositioned::Type ty)
   { return (ty == FGPositioned::FIX); }

  inline const std::string& get_ident() const { return ident(); }
  inline double get_lon() const { return longitude(); }
  inline double get_lat() const { return latitude(); }
};
