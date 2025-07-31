// Cocoa implementation of clipboard access for Nasal
//
// SPDX-FileCopyrightText: 2012 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include "NasalClipboard.hxx"

#include <simgear/debug/logstream.hxx>

#include <AppKit/NSPasteboard.h>
#include <Foundation/NSArray.h>

#include <GUI/CocoaHelpers_private.h>

/**
 */
class ClipboardCocoa: public NasalClipboard
{
  public:

    /**
     * Get clipboard contents as text
     */
    virtual std::string getText(Type type)
    {
      CocoaAutoreleasePool pool;

      if( type == CLIPBOARD )
      {
       NSPasteboard* pboard = [NSPasteboard generalPasteboard];
      NSString* nstext = [pboard stringForType:NSPasteboardTypeString];
       return stdStringFromCocoa(nstext);
      }

      return "";
    }

    /**
     * Set clipboard contents as text
     */
    virtual bool setText(const std::string& text, Type type)
    {
      CocoaAutoreleasePool pool;

      if( type == CLIPBOARD )
      {
        NSPasteboard* pboard = [NSPasteboard generalPasteboard];
        NSString* nstext = stdStringToCocoa(text);
        [pboard clearContents];
        [pboard setString:nstext forType:NSPasteboardTypeString];
        return true;
      }

      return false;
    }

  protected:

    std::string _selection;
};

//------------------------------------------------------------------------------
NasalClipboard::Ptr NasalClipboard::create()
{
  return NasalClipboard::Ptr(new ClipboardCocoa);
}
