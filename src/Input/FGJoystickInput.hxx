// FGJoystickInput.hxx -- handle user input from joystick devices
//
// Written by Torsten Dreyer, started August 2009
// Based on work from David Megginson, started May 2001.
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson <david@megginson.com>
// SPDX-FileCopyrightText: 2009 Torsten Dreyer

#pragma once

#include "FGCommonInput.hxx"
#include "FGButton.hxx"

#include <memory> // for std::unique_ptr
#include <simgear/structure/subsystem_mgr.hxx>

#include "FlightGear_js.h"

////////////////////////////////////////////////////////////////////////
// The Joystick Input Class
////////////////////////////////////////////////////////////////////////
class FGJoystickInput : public SGSubsystem,
                        FGCommonInput
{
public:
    FGJoystickInput();
    virtual ~FGJoystickInput();

    // Subsystem API.
    void init() override;
    void postinit() override;
    void reinit() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "input-joystick"; }

    static const int MAX_JOYSTICKS        = 16;
    static const int MAX_JOYSTICK_AXES    = _JS_MAX_AXES;
    static const int MAX_JOYSTICK_BUTTONS = 32;

private:
    /**
     * @brief computeDeviceIndexName - compute the name including the index, based
     * on the number of identically named devices. This is used to allow multiple
     * different files for identical hardware, especially throttles
     * @param name - the base joystick name
     * @param lastIndex - don't check names at this index or above. Needed to
     * ensure we only check as far as the joystick we are currently processing
     * @return
     */
    std::string computeDeviceIndexName(const std::string &name, int lastIndex) const;

    void _remove(bool all);
    SGPropertyNode_ptr status_node;

    /**
     * Settings for a single joystick axis.
     */
    struct axis {
        axis ();
        virtual ~axis ();
        float last_value;
        float tolerance;
        binding_list_t bindings[KEYMOD_MAX];
        float low_threshold;
        float high_threshold;
        FGButton low;
        FGButton high;
        float interval_sec, delay_sec, release_delay_sec;
        double last_dt;
    };

    /**
     * Settings for a joystick.
     */
    struct joystick {
      joystick ();
      virtual ~joystick ();
      int jsnum;
      std::unique_ptr<jsJoystick> plibJS;
      int naxes;
      int nbuttons;
      axis * axes;
      FGButton * buttons;
      bool predefined;
      bool initializing = true;
      bool initialized = false;
      float values[MAX_JOYSTICK_AXES];
      double init_dt = 0.0f;

      void clearAxesAndButtons();
    };

    joystick joysticks[MAX_JOYSTICKS];
    void updateJoystick(int index, joystick* joy, double dt);
};
