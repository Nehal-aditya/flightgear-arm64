// Expose SGPath module to Nasal
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2013  James Turner

#include "config.h"

#include "simgear/structure/exception.hxx"

#include "NasalSGPath.hxx"
#include <Main/globals.hxx>
#include <simgear/misc/sg_path.hxx>

#include <simgear/nasal/cppbind/Ghost.hxx>
#include <simgear/nasal/cppbind/NasalHash.hxx>

typedef std::shared_ptr<SGPath> SGPathRef;
typedef nasal::Ghost<SGPathRef> NasalSGPath;

// TODO make exposing such function easier...
static naRef validatedPathToNasal(const nasal::CallContext& ctx,
                                  const SGPath& p)
{
    return ctx.to_nasal(SGPathRef(new SGPath(p.utf8Str(),
                                             &SGPath::NasalIORulesChecker)));
}

/**
 * os.path.new()
 */
static naRef f_new_path(const nasal::CallContext& ctx)
{
    return validatedPathToNasal(ctx, SGPath(ctx.getArg<std::string>(0)));
}

static int f_path_create_dir(SGPath& p, const nasal::CallContext& ctx)
{
    // limit setable access rights for Nasal
    return p.create_dir(ctx.getArg<mode_t>(0, 0755) & 0775);
}

static void f_path_set(SGPath& p, const nasal::CallContext& ctx)
{
    p = SGPath::fromUtf8(ctx.getArg<std::string>(0), p.getPermissionChecker());
}

/**
 * os.path.desktop()
 */
static naRef f_desktop(const nasal::CallContext& ctx)
{
    return validatedPathToNasal(
        ctx, SGPath::desktop(SGPath(&SGPath::NasalIORulesChecker)));
}

SGPath::StandardLocation standardLocationFromString(const std::string& s)
{
    if (s == "DESKTOP") return SGPath::DESKTOP;
    if (s == "DOWNLOADS") return SGPath::DOWNLOADS;
    if (s == "DOCUMENTS") return SGPath::DOCUMENTS;
    if (s == "PICTURES") return SGPath::PICTURES;
    if (s == "HOME") return SGPath::HOME;

    throw sg_range_exception("Unrecognized standard location: '" + s + "'");
}

/**
 * os.path.standardLocation(type)
 */
static naRef f_standardLocation(const nasal::CallContext& ctx)
{
    const std::string type_str = ctx.requireArg<std::string>(0);
    try {
        SGPath::StandardLocation type = standardLocationFromString(type_str);
        return validatedPathToNasal(ctx, SGPath::standardLocation(type));
    } catch (sg_exception&) {
        ctx.runtimeError(
            "os.path.standardLocation: unknown type %s", type_str.c_str());
    }
}

//------------------------------------------------------------------------------
naRef initNasalSGPath(naRef globals, naContext c)
{
    // This wraps most of the SGPath APIs for use by Nasal
    // See: http://docs.freeflightsim.org/simgear/classSGPath.html

    NasalSGPath::init("os.path")
        .method("set", &f_path_set)
        .method("append", &SGPath::append)
        .method("concat", &SGPath::concat)

        .member("realpath", &SGPath::realpath)
        .member("file", &SGPath::file)
        .member("dir", &SGPath::dir)
        .member("base", &SGPath::base)
        .member("file_base", &SGPath::file_base)
        .member("extension", &SGPath::extension)
        .member("lower_extension", &SGPath::lower_extension)
        .member("complete_lower_extension", &SGPath::complete_lower_extension)
        .member("str", &SGPath::utf8Str)
        .member("mtime", &SGPath::modTime)

        .method("exists", &SGPath::exists)
        .method("canRead", &SGPath::canRead)
        .method("canWrite", &SGPath::canWrite)
        .method("isFile", &SGPath::isFile)
        .method("isDir", &SGPath::isDir)
        .method("isRelative", &SGPath::isRelative)
        .method("isAbsolute", &SGPath::isAbsolute)
        .method("isNull", &SGPath::isNull)

        .method("create_dir", &f_path_create_dir)
        .method("remove", &SGPath::remove)
        .method("rename", &SGPath::rename);

    nasal::Hash globals_module(globals, c),
        path = globals_module.createHash("os")
                   .createHash("path");

    path.set("new", f_new_path);
    path.set("desktop", &f_desktop);
    path.set("standardLocation", &f_standardLocation);

    return naNil();
}
