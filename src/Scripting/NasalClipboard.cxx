// X11 implementation of clipboard access for Nasal
//
// SPDX-FileCopyrightText: 2012 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "NasalClipboard.hxx"
#include "NasalSys.hxx"
#include <simgear/nasal/cppbind/NasalCallContext.hxx>

#include <cstddef>

/*
 *  Nasal wrappers for setting/getting clipboard text
 */
//------------------------------------------------------------------------------
static NasalClipboard::Type parseType(const nasal::CallContext& ctx, size_t i)
{
  if( ctx.argc > i )
  {
    if( ctx.isNumeric(i) )
    {
      if( ctx.requireArg<int>(i) == NasalClipboard::CLIPBOARD )
        return NasalClipboard::CLIPBOARD;
      if( ctx.requireArg<int>(i) == NasalClipboard::PRIMARY )
        return NasalClipboard::PRIMARY;
    }

    ctx.runtimeError("clipboard: invalid arg "
                     "(expected clipboard.CLIPBOARD or clipboard.SELECTION)");
  }

  return NasalClipboard::CLIPBOARD;
}

//------------------------------------------------------------------------------
static naRef f_setClipboardText(const nasal::CallContext& ctx)
{
  if( ctx.argc < 1 || ctx.argc > 2 )
    ctx.runtimeError("clipboard.setText() expects 1 or 2 arguments: "
                     "text, [, type = clipboard.CLIPBOARD]");

  return
    naNum
    (
      NasalClipboard::getInstance()->setText( ctx.requireArg<std::string>(0),
                                              parseType(ctx, 1) )
    );
}

//------------------------------------------------------------------------------
static naRef f_getClipboardText(const nasal::CallContext& ctx)
{
  if( ctx.argc > 1 )
    ctx.runtimeError("clipboard.getText() accepts max 1 arg: "
                     "[type = clipboard.CLIPBOARD]");

  return ctx.to_nasal
  (
    NasalClipboard::getInstance()->getText(parseType(ctx, 0))
  );
}

//------------------------------------------------------------------------------
NasalClipboard::Ptr NasalClipboard::_clipboard;

//------------------------------------------------------------------------------
NasalClipboard::~NasalClipboard()
{

}

//------------------------------------------------------------------------------
void NasalClipboard::init(FGNasalSys *nasal)
{
  _clipboard = create();

  nasal::Hash clipboard = nasal->getGlobals().createHash("clipboard");

  clipboard.set("setText", f_setClipboardText);
  clipboard.set("getText", f_getClipboardText);
  clipboard.set("CLIPBOARD", NasalClipboard::CLIPBOARD);
  clipboard.set("SELECTION", NasalClipboard::PRIMARY);
}

//------------------------------------------------------------------------------
NasalClipboard::Ptr NasalClipboard::getInstance()
{
  return _clipboard;
}
