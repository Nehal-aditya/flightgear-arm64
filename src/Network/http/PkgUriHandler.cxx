// PkgUriHandler.cxx -- service for the package system
//
// Written by Torsten Dreyer, started February 2015.
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


#include "PkgUriHandler.hxx"

#include <nlohmann/json.hpp>

#include <simgear/package/Root.hxx>
#include <simgear/package/Catalog.hxx>
#include <simgear/package/Delegate.hxx>
#include <simgear/package/Install.hxx>
#include <simgear/package/Package.hxx>

#include <Main/fg_props.hxx>

using nlohmann::json;
using std::string;

static json PackageToJson(simgear::pkg::Package* p)
{
    if (!p) {
        return {};
    }

    return json{
        {"id", p->id()},
        {"name", p->name()},
        {"description", p->description()},
        {"installed", p->isInstalled()},
        {"thumbnails", p->thumbnailUrls()},
        {"variants", p->variants()},
        {"revision", p->revision()},
        {"fileSize", p->fileSizeBytes()},
        {"author", p->getLocalisedProp("author")},
        {"ratingFdm", p->getLocalisedProp("rating/FDM")},
        {"ratingCockpit", p->getLocalisedProp("rating/cockpit")},
        {"ratingModel", p->getLocalisedProp("rating/model")},
        {"ratingSystems", p->getLocalisedProp("rating/systems")}};
}


namespace simgear {
namespace pkg {

// Nlohmann needs adpaters defined in the classes' namespace
void to_json(json& j, const PackageRef& p)
{
    j = PackageToJson(p.get());
}

void to_json(json& j, const CatalogRef& c)
{
    if (!c) {
        return;
    }

    j = json{
        {"id", c->id()},
        {"installRoot", c->installRoot().utf8Str()},
        {"url", c->url()},
        {"description", c->description()},
        {"packages", c->packages()},
        {"needingUpdate", c->packagesNeedingUpdate()},
        {"installed", c->installedPackages()}};
}

} // namespace pkg
} // namespace simgear

namespace flightgear {
namespace http {

/*
url: /pkg/command/args

Examples:
/pkg/path

Input:
{
  command: "command",
  args: {
  }
}

Output:
{
}
*/

static string PackageRootCommand( simgear::pkg::Root* packageRoot, const string & command, const string & args )
{
    json r;

    if (command == "path") {
        r["path"] = packageRoot->path().utf8Str();
    } else if (command == "version") {
        r["version"] = packageRoot->applicationVersion();
    } else if (command == "refresh") {
        packageRoot->refresh(true);
        r["refresh"] = "OK";
    } else if (command == "catalogs") {
        r["catalogs"] = packageRoot->catalogs();
    } else if (command == "packageById") {
        r["package"] = packageRoot->getPackageById(args);
    } else if (command == "catalogById") {
        r["catalog"] = packageRoot->getCatalogById(args);
    } else if (command == "search") {
        SGPropertyNode_ptr query(new SGPropertyNode);
        simgear::pkg::PackageList packageList = packageRoot->packagesMatching(query);
        r["packages"] = packageList;
    } else if (command == "install") {
        simgear::pkg::PackageRef package = packageRoot->getPackageById(args);
        if (NULL == package) {
            SG_LOG(SG_NETWORK, SG_WARN, "Can't install package '" << args << "', package not found");
            return string("");
        }
        package->existingInstall();
    } else {
        SG_LOG(SG_NETWORK, SG_WARN, "Unhandled pkg command : '" << command << "'");
        return string("");
    }

    return r.dump();
    ;
}

static  string findCommand( const string & uri, string & outArgs )
{
  size_t n = uri.find_first_of('/');
  if( n == string::npos ) outArgs = string("");
  else outArgs = uri.substr( n+1 );
  return uri.substr( 0, n );
}

bool PkgUriHandler::handleRequest( const HTTPRequest & request, HTTPResponse & response, Connection * connection )
{
  response.Header["Content-Type"] = "application/json; charset=UTF-8";
  response.Header["Access-Control-Allow-Origin"] = "*";
  response.Header["Access-Control-Allow-Methods"] = "OPTIONS, GET, POST";
  response.Header["Access-Control-Allow-Headers"] = "Origin, Accept, Content-Type, X-Requested-With, X-CSRF-Token";

  if( request.Method == "OPTIONS" ){
      return true; // OPTIONS only needs the headers
  }

  simgear::pkg::Root* packageRoot = globals->packageRoot();
  if( NULL == packageRoot ) {
    SG_LOG( SG_NETWORK,SG_WARN, "NO PackageRoot" );
    response.StatusCode = 500;
    response.Content = "{}";
    return true; 
  }

  string argString;
  string command = findCommand( string(request.Uri).substr(getUri().size()), argString );
  

  SG_LOG(SG_NETWORK,SG_INFO, "Request is for command '"  << command << "' with arg='" << argString << "'" );

  if( request.Method == "GET" ){
  } else if( request.Method == "POST" ) {
  } else {
    SG_LOG(SG_NETWORK,SG_INFO, "PkgUriHandler: invalid request method '" << request.Method << "'" );
    response.Header["Allow"] = "OPTIONS, GET, POST";
    response.StatusCode = 405;
    response.Content = "{}";
    return true; 
  }

  response.Content = PackageRootCommand( packageRoot, command, argString );
  if( response.Content.empty() ) {
    response.StatusCode = 404;
    response.Content = "{}";
  }
  return true; 
}

} // namespace http
} // namespace flightgear

