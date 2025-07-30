// jssuper.h -- manage access to multiple joysticks
//
// Written by Tony Peden, started May 2001
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Tony Peden <apeden@earthlink.net>

#pragma once

#include <config.h>

#include "FlightGear_js.h"

#define MAX_JOYSTICKS 8

class jsSuper {
  private:
    int activeJoysticks;
    int active[MAX_JOYSTICKS];
    int currentJoystick;
    int first, last;
    jsJoystick* js[MAX_JOYSTICKS];

  public:
    jsSuper(void);
    ~jsSuper(void);

    inline int getNumJoysticks(void) { return activeJoysticks; }

    inline int atFirst(void) { return currentJoystick == first; }
    inline int atLast(void) { return currentJoystick == last; }

    inline void firstJoystick(void) { currentJoystick=first; }
    inline void lastJoystick(void) { currentJoystick=last; }

    int nextJoystick(void);
    int prevJoystick(void);

    inline jsJoystick* getJoystick(int Joystick)
            { currentJoystick=Joystick; return js[Joystick]; }

    inline jsJoystick* getJoystick(void) { return js[currentJoystick]; }

    inline int getCurrentJoystickId(void) { return currentJoystick; }
};
