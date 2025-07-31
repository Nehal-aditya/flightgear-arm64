// Clipboard access for Nasal
////
// SPDX-FileCopyrightText: 2012 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/nasal/nasal.h>
#include <memory>
#include <string>

class FGNasalSys;
class NasalClipboard
{
  public:

    enum Type
    {
      /// Standard clipboard as supported by nearly all operating systems
      CLIPBOARD,

      /// X11 platforms support also a mode called PRIMARY selection which
      /// contains the current (mouse) selection and can typically be inserted
      /// via a press on the middle mouse button
      PRIMARY
    };

    typedef std::shared_ptr<NasalClipboard> Ptr;

    virtual void update() {}
    virtual std::string getText(Type type = CLIPBOARD) = 0;
    virtual bool setText( const std::string& text,
                          Type type = CLIPBOARD ) = 0;

    /**
     * Sets up the clipboard and puts all the extension functions into a new
     * "clipboard" namespace.
     */
    static void init(FGNasalSys *nasal);

    /**
     * Get clipboard platform specific instance
     */
    static Ptr getInstance();

  protected:

    static Ptr      _clipboard;

    /**
     * Implementation supplied by actual platform implementation
     */
    static Ptr create();

    virtual ~NasalClipboard() = 0;
};
