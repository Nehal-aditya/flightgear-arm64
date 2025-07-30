// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2007 Durk Talsma

#pragma once

#include "airports_fwd.hxx"

class XMLVisitor; // ffrom easyxml.hxx
class SGPath;

class XMLLoader {
public:
  XMLLoader();
  ~XMLLoader();

  static void load(FGRunwayPreference* p);
  static void load(FGGroundNetwork*  net);
  static void load(FGSidStar*          s);

  static void loadFromStream(FGGroundNetwork* net, std::istream& inData);
  static void loadFromPath(FGGroundNetwork* net, const SGPath& path);

  /**
   * Search the scenery for a file name of the form:
   *   I/C/A/ICAO.filename.xml
   * and parse it as an XML property list, passing the data to the supplied
   * visitor. If no such file could be found, returns false, otherwise returns
   * true. Other failures (malformed XML, etc) with throw an exception.
   */
  static bool loadAirportXMLDataIntoVisitor(const std::string& aICAO,
                                            const std::string& aFileName, XMLVisitor& aVisitor);

  /**
   * Search the scenery for a file name of the form:
   *   I/C/A/ICAO.filename.xml
   * and return the corresponding SGPath if found (and true),
   * or false and invalid path if no matching data could be found
   */
  static bool findAirportData(const std::string& aICAO,
                              const std::string& aFileName, SGPath& aPath);
};
