// jsonprops.hxx -- convert properties from/to json
//
// Written by Torsten Dreyer, started April 2014.
//
// SPDX-FileCopyrightText: Copyright (C) 2014  Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

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
