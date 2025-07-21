// jsonprops.hxx -- convert properties from/to json
//
// Written by Torsten Dreyer, started April 2014.
//
// Copyright (C) 2014  Torsten Dreyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <nlohmann/json_fwd.hpp>
#include <simgear/props/props.hxx>
#include <string>

namespace flightgear {
namespace http {

class JSON {
public:
    static nlohmann::json toJson(SGPropertyNode_ptr n, int depth, double timestamp = -1.0);
    static std::string toJsonString(bool indent, SGPropertyNode_ptr n, int depth, double timestamp = -1.0);

    static const char* getPropertyTypeString(simgear::props::Type type);
    static nlohmann::json valueToJson(SGPropertyNode_ptr n);
    static void setValueFromJSON(const nlohmann::json& json, SGPropertyNode_ptr node);
    static void toProp(const nlohmann::json& json, SGPropertyNode_ptr base);
    static void addChildrenToProp(const nlohmann::json& json, SGPropertyNode_ptr base);
};

}  // namespace http
} // namespace flightgear
