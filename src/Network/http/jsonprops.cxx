// jsonprops.cxx -- convert properties from/to json
//
// Written by Torsten Dreyer, started April 2014.
//
// SPDX-FileCopyrightText: 2014 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "jsonprops.hxx"
#include "simgear/debug/debug_types.h"
#include "simgear/structure/exception.hxx"

#include <nlohmann/json.hpp>

#include <simgear/misc/strutils.hxx>
#include <simgear/math/SGMath.hxx>

namespace flightgear {
namespace http {

using nlohmann::json;
using std::string;

const char * JSON::getPropertyTypeString(simgear::props::Type type)
{
  switch (type) {
    case simgear::props::NONE:
      return "-";

    case simgear::props::ALIAS:
      return "alias";

    case simgear::props::BOOL:
      return "bool";

    case simgear::props::INT:
      return "int";

    case simgear::props::LONG:
      return "long";

    case simgear::props::FLOAT:
      return "float";

    case simgear::props::DOUBLE:
      return "double";

    case simgear::props::STRING:
      return "string";

    case simgear::props::UNSPECIFIED:
      return "unspecified";

    case simgear::props::EXTENDED:
      return "extended";

    case simgear::props::VEC3D:
      return "vec3d";

    case simgear::props::VEC4D:
      return "vec4d";

    default:
      return "?";
  }
}

json JSON::valueToJson(SGPropertyNode_ptr n)
{
    if( !n->hasValue() )
        return json(nullptr);

    switch( n->getType() ) {
        case simgear::props::BOOL:
            return n->getBoolValue();
        case simgear::props::INT:
        case simgear::props::LONG:
        case simgear::props::FLOAT:
        case simgear::props::DOUBLE: {
            double val = n->getDoubleValue();
            return SGMiscd::isNaN(val) ? json(nullptr) : json(val);
        }

        default:
            return n->getStringValue();
    }
}


json JSON::toJson(SGPropertyNode_ptr n, int depth, double timestamp)
{
    const auto nc = n->nChildren();
    json j = {
        {"path", n->getPath(true)},
        {"name", n->getNameString()},
        {"value", valueToJson(n)},
        {"type", getPropertyTypeString(n->getType())},
        {"index", n->getIndex()},
        {"nChildren", nc}};

    if (timestamp > 0.0) {
        j["ts"] = timestamp;
    }

    if ((depth > 0) && (nc > 0)) {
        json children;
        for (int i = 0; i < nc; i++) {
            children.push_back(toJson(n->getChild(i), depth - 1, timestamp));
        }
        j["children"] = children;
    }

    return j;
}

void JSON::toProp(const json& j, SGPropertyNode_ptr base)
{
    if (!j.is_object()) {
        // warn / throw exception?
        return;
    }

  SGPropertyNode_ptr n = base;

  // check if name is set. If so, update child with given name
  // else update base
  if (j.contains("name")) {
      const auto name = simgear::strutils::strip(j.value<std::string>("name", {}));
      if (name.empty()) {
          // error / exception?
      } else {
          const int index = j.value<int>("index", 0);
          n = base->getNode(name, index, true);
      }
  }

  if (j.contains("children")) {
      addChildrenToProp(j, n);
  } else if (j.contains("value")) {
      setValueFromJSON(j.at("value"), n);
  }
}

void JSON::setValueFromJSON(const nlohmann::json& v, SGPropertyNode_ptr n)
{
    if (v.is_boolean()) {
        n->setBoolValue(v.template get<bool>());
    } else if (v.is_number_integer()) {
        n->setIntValue(v.template get<int>());
    } else if (v.is_number_unsigned()) {
        n->setIntValue(v.template get<int>());
    } else if (v.is_number_float()) {
        n->setDoubleValue(v.template get<double>());
    } else if (v.is_string()) {
        n->setStringValue(v.template get<string>());
    } else {
        SG_LOG(SG_IO, SG_DEV_WARN, "setValueFromJSON: could not convert JSON value to SGPropertyNode value:" << v.dump());
    }
}

void JSON::addChildrenToProp(const json& j, SGPropertyNode_ptr n)
{
    if (!n || !j.is_object()) {
        // warn / throw exception?
        return;
    }

    if (!j.contains("children")) {
        return;
    }

    const auto& children = j.at("children");
    for (const auto& c : children) {
        toProp(c, n);
    };
}

string JSON::toJsonString(bool indent, SGPropertyNode_ptr n, int depth, double timestamp )
{
    auto j = toJson(n, depth, timestamp);
    return j.dump(indent, ' ', false, nlohmann::detail::error_handler_t::replace);
}

}  // namespace http
} // namespace flightgear
