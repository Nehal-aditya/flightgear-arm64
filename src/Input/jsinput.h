// jsinput.h -- wait for and identify input from joystick
//
// Written by Tony Peden, started May 2001
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Tony Peden <apeden@earthlink.net>

#pragma once

#include <cmath> // for fabs

#include "jssuper.h"

class jsInput {
  private:
    jsSuper *jss;
    bool pretty_display;
    float axes[_JS_MAX_AXES];
    float axes_iv[MAX_JOYSTICKS][_JS_MAX_AXES];
    int button_iv[MAX_JOYSTICKS];

    int joystick,axis,button;
    bool axis_positive;

    float axis_threshold;

  public:
    jsInput(jsSuper *jss);
    ~jsInput(void);

    inline void displayValues(bool bb) { pretty_display=bb; }

    int getInput(void);
    void findDeadBand(void);

    inline int getInputJoystick(void) { return joystick; }
    inline int getInputAxis(void)     { return axis; }
    inline int getInputButton(void)   { return button; }
    inline bool getInputAxisPositive(void) { return axis_positive; }

    inline float getReturnThreshold(void) { return axis_threshold; }
    inline void setReturnThreshold(float ff)
              { if(fabs(ff) <= 1.0) axis_threshold=ff; }
};
