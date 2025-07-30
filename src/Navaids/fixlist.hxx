// fixlist.hxx -- fix list management class
//
// Written by Curtis Olson, started April 2000.
//
// SPDX-FileCopyrightText: 2000 Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <Navaids/NavDataCache.hxx>
#include <simgear/compiler.h>
#include <simgear/math/SGGeod.hxx>
#include <unordered_map>
#include <string>

class SGPath;
class sg_gzifstream;

namespace flightgear
{
  class FixesLoader
  {
  public:
    FixesLoader();
    ~FixesLoader();

    // Load fixes from the specified fix.dat (or fix.dat.gz) file
    void loadFixes(const NavDataCache::SceneryLocation& sceneryLocation,
                   std::size_t bytesReadSoFar,
                   std::size_t totalSizeOfAllDatFiles);

  private:
    void throwExceptionIfStreamError(const sg_gzifstream& input_stream,
                                     const SGPath& path);

    NavDataCache* _cache;
    std::unordered_multimap<std::string, SGGeod> _loadedFixes;
  };
  } // namespace flightgear
