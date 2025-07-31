// Fallback implementation of clipboard access for Nasal. Copy and edit for
// implementing support of other platforms
//
// SPDX-FileCopyrightText: 2012 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "NasalClipboard.hxx"

#include <simgear/debug/logstream.hxx>


/**
 * Provide a basic clipboard whose contents are only available to FlightGear
 * itself
 */
class ClipboardFallback:
  public NasalClipboard
{
  public:

    /**
     * Get clipboard contents as text
     */
    virtual std::string getText(Type type)
    {
      return type == CLIPBOARD ? _clipboard : _selection;
    }

    /**
     * Set clipboard contents as text
     */
    virtual bool setText(const std::string& text, Type type)
    {
      if( type == CLIPBOARD )
        _clipboard = text;
      else
        _selection = text;
      return true;
    }

  protected:

    std::string _clipboard,
                _selection;
};

//------------------------------------------------------------------------------
NasalClipboard::Ptr NasalClipboard::create()
{
  return NasalClipboard::Ptr(new ClipboardFallback);
}
