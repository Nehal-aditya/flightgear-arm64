// metarairportfilter.cxx -- Implementation of AirportFilter
//
// Written by Torsten Dreyer, August 2010
//
// SPDX-FileCopyrightText: 2010 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#include <config.h>

#include "metarairportfilter.hxx"

namespace Environment {

MetarAirportFilter * MetarAirportFilter::_instance = NULL;

MetarAirportFilter * MetarAirportFilter::instance()
{
  return _instance != NULL ? _instance :
    (_instance = new MetarAirportFilter());
}

} // namespace Environment
