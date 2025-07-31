// Expose HTTP module to Nasal
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "NasalHTTP.hxx"
#include <Main/globals.hxx>
#include <Network/HTTPClient.hxx>

#include <simgear/io/HTTPFileRequest.hxx>
#include <simgear/io/HTTPMemoryRequest.hxx>

#include <simgear/nasal/cppbind/from_nasal.hxx>
#include <simgear/nasal/cppbind/to_nasal.hxx>
#include <simgear/nasal/cppbind/NasalHash.hxx>
#include <simgear/nasal/cppbind/Ghost.hxx>

typedef nasal::Ghost<simgear::HTTP::Request_ptr> NasalRequest;
typedef nasal::Ghost<simgear::HTTP::FileRequestRef> NasalFileRequest;
typedef nasal::Ghost<simgear::HTTP::MemoryRequestRef> NasalMemoryRequest;

FGHTTPClient& requireHTTPClient(const nasal::ContextWrapper& ctx)
{
  auto http = globals->get_subsystem<FGHTTPClient>();
  if( !http )
    ctx.runtimeError("Failed to get HTTP subsystem");

  return *http;
}

/**
 * http.save(url, filename)
 */
static naRef f_http_save(const nasal::CallContext& ctx)
{
  const std::string url = ctx.requireArg<std::string>(0);

  // Check for write access to target file
  const std::string filename = ctx.requireArg<std::string>(1);
  const SGPath validated_path = SGPath(filename).validate(true);

  if( validated_path.isNull() )
    ctx.runtimeError("Access denied: can not write to %s", filename.c_str());

  return ctx.to_nasal
  (
    requireHTTPClient(ctx).client()->save(url, validated_path.utf8Str())
  );
}

/**
 * http.load(url)
 */
static naRef f_http_load(const nasal::CallContext& ctx)
{
  const std::string url = ctx.requireArg<std::string>(0);
  return ctx.to_nasal( requireHTTPClient(ctx).client()->load(url) );
}

static naRef f_request_abort( simgear::HTTP::Request&,
                              const nasal::CallContext& ctx )
{
    // we need a request_ptr for cancel, not a reference. So extract
    // the me object from the context directly.
    simgear::HTTP::Request_ptr req = ctx.from_nasal<simgear::HTTP::Request_ptr>(ctx.me);
    requireHTTPClient(ctx).client()->cancelRequest(req);
    return naNil();
}

//------------------------------------------------------------------------------
naRef initNasalHTTP(naRef globals, naContext c)
{
  using simgear::HTTP::Request;
  typedef Request* (Request::*HTTPCallback)(const Request::Callback&);
  NasalRequest::init("http.Request")
    .member("url", &Request::url)
    .member("method", &Request::method)
    .member("scheme", &Request::scheme)
    .member("path", &Request::path)
    .member("host", &Request::host)
    .member("port", &Request::port)
    .member("query", &Request::query)
    .member("status", &Request::responseCode)
    .member("reason", &Request::responseReason)
    .member("readyState", &Request::readyState)
    .method("abort", f_request_abort)
    .method("done", static_cast<HTTPCallback>(&Request::done))
    .method("fail", static_cast<HTTPCallback>(&Request::fail))
    .method("always", static_cast<HTTPCallback>(&Request::always));

  using simgear::HTTP::FileRequest;
  NasalFileRequest::init("http.FileRequest")
    .bases<NasalRequest>();

  using simgear::HTTP::MemoryRequest;
  NasalMemoryRequest::init("http.MemoryRequest")
    .bases<NasalRequest>()
    .member("response", &MemoryRequest::responseBody);

  nasal::Hash globals_module(globals, c),
              http = globals_module.createHash("http");

  http.set("save", f_http_save);
  http.set("load", f_http_load);

  return naNil();
}
