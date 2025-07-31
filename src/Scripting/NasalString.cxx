// Add (std::string) like methods to Nasal strings
//
// SPDX-FileCopyrightText: 2013 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "NasalString.hxx"

#include <simgear/nasal/cppbind/from_nasal.hxx>
#include <simgear/nasal/cppbind/Ghost.hxx>
#include <simgear/nasal/cppbind/NasalHash.hxx>
#include <simgear/nasal/cppbind/NasalString.hxx>

/**
 *  Compare (sub)string with other string
 *
 *  compare(s)
 *  compare(pos, len, s)
 */
static naRef f_compare(const nasal::CallContext& ctx)
{
  nasal::String str = ctx.from_nasal<nasal::String>(ctx.me),
                rhs = ctx.requireArg<nasal::String>(ctx.argc > 1 ? 2 : 0);
  size_t pos = ctx.argc > 1 ? ctx.requireArg<int>(1) : 0;
  size_t len = ctx.argc > 1 ? ctx.requireArg<int>(2) : 0;

  if( len == 0 )
    len = nasal::String::npos;

  return naNum( str.compare(pos, len, rhs) );
}

/**
 *  Check whether string starts with other string
 */
static naRef f_starts_with(const nasal::CallContext& ctx)
{
  nasal::String str = ctx.from_nasal<nasal::String>(ctx.me),
                rhs = ctx.requireArg<nasal::String>(0);

  return naNum( str.starts_with(rhs) );
}

/**
 *  Check whether string ends with other string
 */
static naRef f_ends_with(const nasal::CallContext& ctx)
{
  nasal::String str = ctx.from_nasal<nasal::String>(ctx.me),
                rhs = ctx.requireArg<nasal::String>(0);

  return naNum( str.ends_with(rhs) );
}

/**
 *  Helper to convert size_t position/npos to Nasal conventions (-1 == npos)
 */
naRef pos_to_nasal(size_t pos)
{
  if( pos == nasal::String::npos )
    return naNum(-1);
  else
    return naNum(pos);
}

/**
 *  Find first occurrence of single character
 *
 *  find(c, pos = 0)
 */
static naRef f_find(const nasal::CallContext& ctx)
{
  nasal::String str = ctx.from_nasal<nasal::String>(ctx.me),
                find = ctx.requireArg<nasal::String>(0);
  size_t pos = ctx.getArg<int>(1, 0);

  if( find.size() != 1 )
    ctx.runtimeError("string::find: single character expected");

  return pos_to_nasal( str.find(*find.c_str(), pos) );
}

/**
 * Find first character of a string occurring in this string
 *
 * find_first_of(search, pos = 0)
 */
static naRef f_find_first_of(const nasal::CallContext& ctx)
{
  nasal::String str = ctx.from_nasal<nasal::String>(ctx.me),
                find = ctx.requireArg<nasal::String>(0);
  size_t pos = ctx.getArg<int>(1, 0);

  return pos_to_nasal( str.find_first_of(find, pos) );
}

/**
 * Find first character of this string not occurring in the other string
 *
 * find_first_not_of(search, pos = 0)
 */
static naRef f_find_first_not_of(const nasal::CallContext& ctx)
{
  nasal::String str = ctx.from_nasal<nasal::String>(ctx.me),
                find = ctx.requireArg<nasal::String>(0);
  size_t pos = ctx.getArg<int>(1, 0);

  return pos_to_nasal( str.find_first_not_of(find, pos) );
}

//------------------------------------------------------------------------------
naRef initNasalString(naRef globals, naRef string, naContext c)
{
  nasal::Hash string_module(string, c);

  string_module.set("compare", f_compare);
  string_module.set("starts_with", f_starts_with);
  string_module.set("ends_with", f_ends_with);
  string_module.set("find", f_find);
  string_module.set("find_first_of", f_find_first_of);
  string_module.set("find_first_not_of", f_find_first_not_of);

  return naNil();
}
