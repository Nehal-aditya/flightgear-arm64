// Windows implementation of clipboard access for Nasal
//
// SPDX-FileCopyrightText: 2012 Thomas Geymayer <tomgey@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "NasalClipboard.hxx"

#include <simgear/debug/logstream.hxx>
#include <windows.h>

/**
 * Windows does only support on clipboard and no selection. We fake also the X11
 * selection buffer - at least inside FlightGear
 */
class ClipboardWindows:
  public NasalClipboard
{
  public:

    /**
     * Get clipboard contents as text
     */
    virtual std::string getText(Type type)
    {
      if( type == CLIPBOARD )
      {
        std::string data;

        if( !OpenClipboard(NULL) )
          return data;

        HANDLE hData = GetClipboardData( CF_TEXT );
        char* buff = (char*)GlobalLock( hData );
        if (buff)
          data = buff;
        GlobalUnlock( hData );
        CloseClipboard();

        return data;
      }
      else
        return _selection;
    }

    /**
     * Set clipboard contents as text
     */
    virtual bool setText(const std::string& text, Type type)
    {
      if( type == CLIPBOARD )
      {
        if( !OpenClipboard(NULL) )
          return false;

        bool ret = true;
        if( !EmptyClipboard() )
          ret = false;
        else if( !text.empty() )
        {
          HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
          if( !hGlob )
            ret = false;
          else
          {
            memcpy(GlobalLock(hGlob), (char*)&text[0], text.size() + 1);
            GlobalUnlock(hGlob);

            if( !SetClipboardData(CF_TEXT, hGlob) )
            {
              GlobalFree(hGlob);
              ret = false;
            }
          }
        }

        CloseClipboard();
        return ret;
      }
      else
      {
        _selection = text;
        return true;
      }
    }

  protected:

    std::string _selection;
};

//------------------------------------------------------------------------------
NasalClipboard::Ptr NasalClipboard::create()
{
  return NasalClipboard::Ptr(new ClipboardWindows);
}
