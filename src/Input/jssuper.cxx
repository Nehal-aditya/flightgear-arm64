// jssuper.cxx -- manage access to multiple joysticks
//
// Written by Tony Peden, started May 2001
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Tony Peden <apeden@earthlink.net>

#include "jssuper.h"


jsSuper::jsSuper(void) {
  int i;

  activeJoysticks=0;
  currentJoystick=0;
  first=-1;
  last=0;
  for ( i = 0; i < MAX_JOYSTICKS; i++ )
      js[i] = new jsJoystick(i);

  for (i = 0; i < MAX_JOYSTICKS; i++) {
      active[i] = !(js[i]->notWorking());
      if (active[i]) {
          activeJoysticks++;
          if (first < 0) {
              first = i;
          }
          last = i;
      }
  }
}


int jsSuper::nextJoystick(void)
{
    // int i;
    if (!activeJoysticks) return 0;
    if (currentJoystick == last) return 0;
    currentJoystick++;
    while (!active[currentJoystick]) { currentJoystick++; };
    return 1;
}

int jsSuper::prevJoystick(void)
{
    // int i;
    if (!activeJoysticks) return 0;
    if (currentJoystick == first) return 0;
    currentJoystick--;
    while (!active[currentJoystick]) { currentJoystick--; };
    return 1;
}


jsSuper::~jsSuper(void) {
  int i;
  for ( i = 0; i < MAX_JOYSTICKS; i++ )
      delete js[i];
}
